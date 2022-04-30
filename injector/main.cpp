#include <Windows.h>
#include <filesystem>
#include <TlHelp32.h>
#include <iostream>
#include <thread>

#define DLL_NAME "killaura.host.dll"
#define PROCESS_NAME L"csgo.exe"

uint32_t get_process_info( const wchar_t* name )
{
	auto snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

	if ( snapshot == INVALID_HANDLE_VALUE )
		return false;

	auto entry = PROCESSENTRY32{ sizeof( PROCESSENTRY32 ) };

	if ( Process32First( snapshot, &entry ) )
	{
		do
		{
			if ( !wcscmp( entry.szExeFile, name ) )
			{
				CloseHandle( snapshot );
				return entry.th32ProcessID;
			}
		} while ( Process32Next( snapshot, &entry ) );
	}

	CloseHandle( snapshot );
	return 0;
}

BOOL main( int arg_number, char* arguments[] )
{
	HANDLE process_handle;
	LPVOID path_alloc;
	HANDLE remote_thread;

	auto cleanup = [ & ]() -> void
	{
		if ( path_alloc )
			VirtualFreeEx( process_handle, path_alloc, 0, MEM_RELEASE );

		if ( remote_thread )
			CloseHandle( remote_thread );

		if ( process_handle )
			CloseHandle( process_handle );
	};

	try
	{
		auto file_path = std::wstring( L"" );

		if ( arg_number == 2 )
		{
			file_path = std::filesystem::canonical( arguments[ 1 ] ).wstring();
		}
		else
		{
			file_path = std::filesystem::canonical( DLL_NAME ).wstring();
		}

		if ( !std::filesystem::exists( file_path ) )
			throw std::runtime_error( "Failed to find target file" );

		auto process_id = get_process_info( PROCESS_NAME );
		if (!process_id)
		{
			throw std::runtime_error("Target process isn't open");
		}

		process_handle = OpenProcess( PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD, FALSE, process_id );
		if (!process_handle)
			throw std::runtime_error("Failed to open process handle");

		path_alloc = VirtualAllocEx( process_handle, NULL, file_path.size() * 2, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
		if ( !path_alloc )
			throw std::runtime_error( "Failed to allocate memory for file path" );

		if ( !WriteProcessMemory( process_handle, path_alloc, file_path.c_str(), file_path.size() * 2, nullptr ) )
			throw std::runtime_error( "Failed to write dll path into process" );

		remote_thread = CreateRemoteThread( process_handle, NULL, NULL, reinterpret_cast< LPTHREAD_START_ROUTINE >( LoadLibraryW ), path_alloc, NULL, NULL );
		if ( !remote_thread )
			throw std::runtime_error( "Failed to open remote thread" );

		WaitForSingleObject( remote_thread, INFINITE );

		std::cout << "Successfully injected!" << std::endl;

		cleanup();

		std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );;
	}
	catch ( const std::runtime_error & err )
	{
		cleanup();

		MessageBoxA( NULL, err.what(), "Error", MB_OK | MB_ICONERROR );

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}