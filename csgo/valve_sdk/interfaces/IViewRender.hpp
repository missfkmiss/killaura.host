#pragma once

enum DrawFlags_t
{
    DF_RENDER_REFRACTION = 0x1,
    DF_RENDER_REFLECTION = 0x2,
    DF_CLIP_Z = 0x4,
    DF_CLIP_BELOW = 0x8,
    DF_RENDER_UNDERWATER = 0x10,
    DF_RENDER_ABOVEWATER = 0x20,
    DF_RENDER_WATER = 0x40,
    DF_UNUSED1 = 0x100,
    DF_WATERHEIGHT = 0x200,
    DF_UNUSED2 = 0x400,
    DF_DRAWSKYBOX = 0x800,
    DF_FUDGE_UP = 0x1000,
    DF_DRAW_ENTITITES = 0x2000,
    DF_UNUSED3 = 0x4000,
    DF_UNUSED4 = 0x8000,
    DF_UNUSED5 = 0x10000,
    DF_SAVEGAMESCREENSHOT = 0x20000,
    DF_CLIP_SKYBOX = 0x40000,
    DF_SHADOW_DEPTH_MAP = 0x100000	// Currently rendering a shadow depth map
};


//-----------------------------------------------------------------------------
// Purpose: View setup and rendering
//-----------------------------------------------------------------------------
class CViewSetup;
class C_BaseEntity;
struct vrect_t;
class C_BaseViewModel;
class IMaterial;

class IViewRender
{
public:
    virtual void                Init(void) = 0;
    virtual void                LevelInit(void) = 0;
    virtual void                LevelShutdown(void) = 0;
    virtual void                Shutdown(void) = 0;
    virtual void                OnRenderStart() = 0;
    virtual	void                Render(vrect_t *rect) = 0;
    virtual void                RenderView(const CViewSetup &view, int nClearFlags, int whatToDraw) = 0;
    virtual int                 GetDrawFlags() = 0;
    virtual void                StartPitchDrift(void) = 0;
    virtual void                StopPitchDrift(void) = 0;
    virtual void*               GetFrustum() = 0;
    virtual bool                ShouldDrawBrushModels(void) = 0;
    virtual const CViewSetup*   GetPlayerViewSetup(void) const = 0;
    virtual const CViewSetup*   GetViewSetup(void) const = 0;
    virtual void                DisableVis(void) = 0;
    virtual int                 BuildWorldListsNumber() const = 0;
    virtual void                SetCheapWaterStartDistance(float flCheapWaterStartDistance) = 0;
    virtual void                SetCheapWaterEndDistance(float flCheapWaterEndDistance) = 0;
    virtual void                GetWaterLODParams(float &flCheapWaterStartDistance, float &flCheapWaterEndDistance) = 0;
    virtual void                DriftPitch(void) = 0;
    virtual void                SetScreenOverlayMaterial(IMaterial *pMaterial) = 0;
    virtual IMaterial*          GetScreenOverlayMaterial() = 0;
    virtual void                WriteSaveGameScreenshot(const char *pFilename) = 0;
    virtual void                WriteSaveGameScreenshotOfSize(const char *pFilename, int width, int height) = 0;
    virtual void                QueueOverlayRenderView(const CViewSetup &view, int nClearFlags, int whatToDraw) = 0;
    virtual float               GetZNear() = 0;
    virtual float               GetZFar() = 0;
    virtual void                GetScreenFadeDistances(float *min, float *max) = 0;
    virtual C_BaseEntity*       GetCurrentlyDrawingEntity() = 0;
    virtual void                SetCurrentlyDrawingEntity(C_BaseEntity *pEnt) = 0;
};


enum
{
	TE_BEAMPOINTS = 0x00,		// beam effect between two points
	TE_SPRITE = 0x01,	// additive sprite, plays 1 cycle
	TE_BEAMDISK = 0x02,	// disk that expands to max radius over lifetime
	TE_BEAMCYLINDER = 0x03,		// cylinder that expands to max radius over lifetime
	TE_BEAMFOLLOW = 0x04,		// create a line of decaying beam segments until entity stops moving
	TE_BEAMRING = 0x05,		// connect a beam ring to two entities
	TE_bmsPLINE = 0x06,
	TE_BEAMRINGPOINT = 0x07,
	TE_BEAMLASER = 0x08,		// Fades according to viewpoint
	TE_BEAMTESLA = 0x09,
};
enum
{
	FBEAM_STARTENTITY = 0x00000001,
	FBEAM_ENDENTITY = 0x00000002,
	FBEAM_FADEIN = 0x00000004,
	FBEAM_FADEOUT = 0x00000008,
	FBEAM_SINENOISE = 0x00000010,
	FBEAM_SOLID = 0x00000020,
	FBEAM_SHADEIN = 0x00000040,
	FBEAM_SHADEOUT = 0x00000080,
	FBEAM_ONLYNOISEONCE = 0x00000100,		// Only calculate our noise once
	FBEAM_NOTILE = 0x00000200,
	FBEAM_USE_HITBOXES = 0x00000400,		// Attachment indices represent hitbox indices instead when this is set.
	FBEAM_STARTVISIBLE = 0x00000800,		// Has this client actually seen this beam's start entity yet?
	FBEAM_ENDVISIBLE = 0x00001000,		// Has this client actually seen this beam's end entity yet?
	FBEAM_ISACTIVE = 0x00002000,
	FBEAM_FOREVER = 0x00004000,
	FBEAM_HALOBEAM = 0x00008000,		// When drawing a beam with a halo, don't ignore the segments and endwidth
	FBEAM_REVERSED = 0x00010000,
	NUM_BEAM_FLAGS = 17	// KEEP THIS UPDATED!
};

struct BeamTrail_t
{
	// NOTE:  Don't add user defined fields except after these four fields.
	BeamTrail_t* next;
	float			die;
	Vector			org;
	Vector			vel;
};

#define NOISE_DIVISIONS		128
#define MAX_BEAM_ENTS		10


typedef int pixelvis_handle_t;

class Beam_t
{
public:
	Beam_t();

	// Methods of IClientRenderable
	virtual const Vector& GetRenderOrigin(void) = 0;
	virtual const QAngle& GetRenderAngles(void) = 0;
	virtual const matrix3x4_t& RenderableToWorldTransform() = 0;
	virtual void			GetRenderBounds(Vector& mins, Vector& maxs) = 0;
	virtual bool			ShouldDraw(void) = 0;
	virtual bool			IsTransparent(void) = 0;
	virtual int				DrawModel(int flags) = 0;
	virtual void			ComputeFxBlend() = 0;
	virtual int				GetFxBlend() = 0;

	// Resets the beam state
	void			Reset();

	// Method to computing the bounding box
	void			ComputeBounds();

	// Bounding box...
	Vector			m_Mins;
	Vector			m_Maxs;
	pixelvis_handle_t* m_queryHandleHalo;
	float			m_haloProxySize;

	// Data is below..

	// Next beam in list
	Beam_t* next;

	// Type of beam
	int				type;
	int				flags;

	// Control points for the beam
	int				numAttachments;
	Vector			attachment[MAX_BEAM_ENTS];
	Vector			delta;

	// 0 .. 1 over lifetime of beam
	float			t;
	float			freq;

	// Time when beam should die
	float			die;
	float			width;
	float			endWidth;
	float			fadeLength;
	float			amplitude;
	float			life;

	// Color
	float			r, g, b;
	float			brightness;

	// Speed
	float			speed;

	// Animation
	float			frameRate;
	float			frame;
	int				segments;

	// Attachment entities for the beam
	HANDLE			entity[MAX_BEAM_ENTS];
	int				attachmentIndex[MAX_BEAM_ENTS];

	// Model info
	int				modelIndex;
	int				haloIndex;

	float			haloScale;
	int				frameCount;

	float			rgNoise[NOISE_DIVISIONS + 1];

	// Popcorn trail for beam follows to use
	BeamTrail_t* trail;

	// for TE_BEAMRINGPOINT
	float			start_radius;
	float			end_radius;

	// for FBEAM_ONLYNOISEONCE
	bool			m_bCalculatedNoise;

	float			m_flHDRColorScale;

};

struct BeamInfo_t
{
	//Beam
	int				m_nType;
	C_BaseEntity* m_pStartEnt;
	int				m_nStartAttachment;
	C_BaseEntity* m_pEndEnt;
	int				m_nEndAttachment;
	Vector			m_vecStart;
	Vector			m_vecEnd;
	int				m_nModelIndex;
	const char* m_pszModelName;
	int				m_nHaloIndex;
	const char* m_pszHaloName;
	float			m_flHaloScale;
	float			m_flLife;
	float			m_flWidth;
	float			m_flEndWidth;
	float			m_flFadeLength;
	float			m_flAmplitude;
	float			m_flBrightness;
	float			m_flSpeed;
	int				m_nStartFrame;
	float			m_flFrameRate;
	float			m_flRed;
	float			m_flGreen;
	float			m_flBlue;
	bool			m_bRenderable;
	int				m_nSegments;
	int				m_nFlags;
	// Rings
	Vector			m_vecCenter;
	float			m_flStartRadius;
	float			m_flEndRadius;

	BeamInfo_t()
	{
		m_nType = TE_BEAMPOINTS;
		m_nSegments = -1;
		m_pszModelName = NULL;
		m_pszHaloName = NULL;
		m_nModelIndex = -1;
		m_nHaloIndex = -1;
		m_bRenderable = true;
		m_nFlags = 0;
	}
};

class C_Beam;
class ITraceFilter;
class IViewRenderBeams
{
public:
	virtual void	InitBeams(void) = 0;
	virtual void	ShutdownBeams(void) = 0;
	virtual void	ClearBeams(void) = 0;

	// Updates the state of the temp ent beams
	virtual void	UpdateTempEntBeams() = 0;

	virtual void	DrawBeam(C_Beam* pbeam, ITraceFilter* pEntityBeamTraceFilter = NULL) = 0;
	virtual void	DrawBeam(Beam_t* pbeam) = 0;

	virtual void	KillDeadBeams(C_BaseEntity* pEnt) = 0;

	// New interfaces!
	virtual Beam_t* CreateBeamEnts(BeamInfo_t& beamInfo) = 0;
	virtual Beam_t* CreateBeamEntPoint(BeamInfo_t& beamInfo) = 0;
	virtual	Beam_t* CreateBeamPoints(BeamInfo_t& beamInfo) = 0;
	virtual Beam_t* CreateBeamRing(BeamInfo_t& beamInfo) = 0;
	virtual Beam_t* CreateBeamRingPoint(BeamInfo_t& beamInfo) = 0;
	virtual Beam_t* CreateBeamCirclePoints(BeamInfo_t& beamInfo) = 0;
	virtual Beam_t* CreateBeamFollow(BeamInfo_t& beamInfo) = 0;

	virtual void	FreeBeam(Beam_t* pBeam) = 0;
	virtual void	UpdateBeamInfo(Beam_t* pBeam, BeamInfo_t& beamInfo) = 0;

	// These will go away!
	virtual void	CreateBeamEnts(int startEnt, int endEnt, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float m_nEndWidth, float m_nFadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b, int type = -1) = 0;
	virtual void	CreateBeamEntPoint(int	nStartEntity, const Vector* pStart, int nEndEntity, const Vector* pEnd,
		int modelIndex, int haloIndex, float haloScale,
		float life, float width, float m_nEndWidth, float m_nFadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b) = 0;
	virtual void	CreateBeamPoints(Vector& start, Vector& end, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float m_nEndWidth, float m_nFadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b) = 0;
	virtual void	CreateBeamRing(int startEnt, int endEnt, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float m_nEndWidth, float m_nFadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b, int flags = 0) = 0;
	virtual void	CreateBeamRingPoint(const Vector& center, float start_radius, float end_radius, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float m_nEndWidth, float m_nFadeLength, float amplitude,
		float brightness, float speed, int startFrame,
		float framerate, float r, float g, float b, int flags = 0) = 0;
	virtual void	CreateBeamCirclePoints(int type, Vector& start, Vector& end,
		int modelIndex, int haloIndex, float haloScale, float life, float width,
		float m_nEndWidth, float m_nFadeLength, float amplitude, float brightness, float speed,
		int startFrame, float framerate, float r, float g, float b) = 0;
	virtual void	CreateBeamFollow(int startEnt, int modelIndex, int haloIndex, float haloScale,
		float life, float width, float m_nEndWidth, float m_nFadeLength, float r, float g, float b,
		float brightness) = 0;
};

extern IViewRenderBeams* g_pBeams;