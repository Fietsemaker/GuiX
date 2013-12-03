#include <GuiX/Config.h>
#include <GuiX/Interfaces.h>
#include <GuiX/Resources.h>

#include <Src/TextureImp.h>

namespace guix {
namespace graphics {

namespace {

static const char* LOG_TAG = "TextureDB";

}; // anonymous namespace

// ===================================================================================
// GxTexture
// ===================================================================================

GxTexture::~GxTexture()
{
	Destroy();
}

GxTexture::GxTexture()
	:myHandle(0)
	,myUVs(NULL)
	,mySize(0,0)
{
}

GxTexture::GxTexture(const char* path)
	:myHandle(0)
	,myUVs(NULL)
	,mySize(0,0)
{
	Create(path);
}

GxTexture::GxTexture(int width, int height, const uchar* pixeldata)
	:myHandle(0)
	,myUVs(NULL)
	,mySize(0,0)
{
	Create(width, height, pixeldata);
}

GxTexture::GxTexture(const GxTexture& other)
	:myHandle(other.myHandle)
	,myUVs(NULL)
	,mySize(other.mySize)
{
	GxTextureDatabaseImp* database = GxTextureDatabaseImp::singleton;
	if(database)
		database->AddReference(myHandle);

	if(other.myUVs)
		mySetUVs(other.myUVs);
}

bool GxTexture::Create(const char* path)
{
	GxTextureDatabaseImp* database = GxTextureDatabaseImp::singleton;
	if(database)
	{
		GxTextureHandle oldHandle = myHandle;
		database->Load(myHandle, mySize, path);
		database->Release(oldHandle);
	}
	else Destroy();

	return (myHandle != 0);
}

bool GxTexture::Create(int width, int height, const uchar* pixeldata)
{
	GxTextureDatabaseImp* database = GxTextureDatabaseImp::singleton;
	if(database)
	{
		GxTextureHandle oldHandle = myHandle;
		if(database->Create(myHandle, width, height, pixeldata))
		{
			mySize.x = width;
			mySize.y = height;
		}
		database->Release(oldHandle);
	}
	else Destroy();

	return (myHandle != 0);
}

void GxTexture::Destroy()
{
	GxTextureDatabaseImp* database = GxTextureDatabaseImp::singleton;
	if(database)
		database->Release(myHandle);

	myHandle = 0;
	mySize.Set(0, 0);
	mySetUVs(NULL);
}

void GxTexture::Assign(const GxTexture& other)
{
	if(this == &other) return;

	GxTextureDatabaseImp* database = GxTextureDatabaseImp::singleton;
	if(database)
	{
		database->AddReference(other.myHandle);
		database->Release(myHandle);
	}
	myHandle = other.myHandle;
	mySize   = other.mySize;
	mySetUVs(other.myUVs);
}

void GxTexture::SetUVs(float u1, float v1, float u2, float v2)
{
	const GxAreaf uvs(u1, v1, u2, v2);
	mySetUVs(&uvs);
}

void GxTexture::SetUVs(const GxAreaf& uvs)
{
	mySetUVs(&uvs);
}

float GxTexture::GetWidthU() const
{
	if(myUVs)
		return (myUVs->r - myUVs->l) * (float)mySize.x;
	return (float)mySize.x;
}

float GxTexture::GetHeightV() const
{
	if(myUVs)
		return (myUVs->b - myUVs->t) * (float)mySize.y;
	return (float)mySize.y;
}

int GxTexture::GetWidth() const
{
	return mySize.x;
}

int GxTexture::GetHeight() const
{
	return mySize.y;
}

GxVec2i GxTexture::GetSize() const
{
	return mySize;
}

GxAreaf GxTexture::GetUVs() const
{
	return myUVs ? (*myUVs) : GxAreaf(0,0,1,1);
}

GxTextureHandle GxTexture::GetHandle() const
{
	return myHandle;
}

GxTexture& GxTexture::operator = (const GxTexture& other)
{
	Assign(other);
	return *this;
}

void GxTexture::mySetUVs(const GxAreaf* uvs)
{
	if(uvs)
	{
		if(myUVs)
		{
			*myUVs = *uvs;
		}
		else
		{
			myUVs = new GxAreaf(*uvs);
		}
	}
	else if(myUVs)
	{
		delete myUVs;
		myUVs = NULL;
	}
}

// ===================================================================================
// GxTextureDatabase
// ===================================================================================

GxTextureDatabaseImp* GxTextureDatabaseImp::singleton = NULL;

GxTextureDatabase::~GxTextureDatabase()
{
}

GxTextureDatabase* GxTextureDatabase::Get()
{
	return GxTextureDatabaseImp::singleton;
}

void GxTextureDatabaseImp::Create()
{
	singleton = new GxTextureDatabaseImp();
}

void GxTextureDatabaseImp::Destroy()
{
	delete singleton;
	singleton = NULL;
}

GxTextureDatabaseImp::GxTextureDatabaseImp()
{
}

GxTextureDatabaseImp::~GxTextureDatabaseImp()
{
	GxRenderInterface* render = GxRenderInterface::Get();

	TexMap::LoadMap::iterator it = myMap.loaded.begin();
	for(; it != myMap.loaded.end(); ++it)
		render->ReleaseTexture(it->second.handle);
}

bool GxTextureDatabaseImp::Load(GxTextureHandle& outHandle, GxVec2i& outSize, const char* path_or_resource)
{
	outHandle = outSize.x = outSize.y = 0;
	if(!path_or_resource) return false;

	GxResources::TextureRes res;
	if(GxResources::Get()->GetResource(path_or_resource, res))
		return myLoadFile(outHandle, outSize, res.path.Raw());

	return myLoadFile(outHandle, outSize, path_or_resource);
}

bool GxTextureDatabaseImp::myLoadFile(GxTextureHandle& outHandle, GxVec2i& outSize, const char* path)
{
	TexMap::Ref* ref = myMap.GetData(path);
	if(ref)
	{
		outHandle = ref->handle;
		outSize.x = ref->data.x;
		outSize.y = ref->data.y;
		++ref->count;
	}
	else
	{
		if(GxRenderInterface::Get()->LoadTexture(outHandle, outSize.x, outSize.y, path))
		{
			myMap.Insert(outHandle, outSize, path);
		}
		else
		{
			GxLog(LOG_TAG, GX_LT_WARNING, "Failed to load \"%s\"", path);
		}
	}
	return (outHandle != 0);
}

bool GxTextureDatabaseImp::Create(GxTextureHandle& outHandle, int width, int height, const uchar* pixeldata)
{
	outHandle = 0;

	if(pixeldata && GxRenderInterface::Get()->GenerateTexture(outHandle, width, height, pixeldata))
	{
		myMap.Insert(outHandle, GxVec2i(width, height));
	}
	else
	{
		GxLog(LOG_TAG, GX_LT_WARNING, "Failed to generate texture (%i, %i)", width, height);
	}

	return (outHandle != 0);
}

void GxTextureDatabaseImp::AddReference(GxTextureHandle handle)
{
	myMap.AddReference(handle);
}

void GxTextureDatabaseImp::Release(GxTextureHandle handle)
{
	TexMap::Ref* ref = myMap.Release(handle);
	if(ref)
	{
		GxRenderInterface::Get()->ReleaseTexture(handle);
		myMap.Erase(handle);
	}
}

int GxTextureDatabaseImp::GetLoadedTextureCount() const
{
	return myMap.GetLoadedCount();
}

void GxTextureDatabaseImp::LogInfo() const
{
	GxLog("Loaded files: %i", myMap.files.size());
	{
		TexMap::FileMap::const_iterator it = myMap.files.begin();
		for(; it != myMap.files.end(); ++it)
			GxLog(" path=\"%s\" handle=%i",
				it->first.Raw(),
				it->second);
		GxLog("");
	}
	GxLog("Loaded textures: %i", myMap.loaded.size());
	{
		TexMap::LoadMap::const_iterator it = myMap.loaded.begin();
		for(; it != myMap.loaded.end(); ++it)
			GxLog(" handle=%i references=%i size=%i, %i",
				it->first, it->second.count,
				it->second.data.x,
				it->second.data.y);
		GxLog("");
	}
}

}; // namespace graphics
}; // namespace guix