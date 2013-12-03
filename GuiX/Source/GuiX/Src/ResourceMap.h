#pragma once

#include <GuiX/Config.h>

#include <map>

#include <GuiX/String.h>

namespace guix {
namespace core {

template <typename _Handle, typename _Data>
class ResourceMap
{
public:
	typedef _Handle Handle;
	typedef _Data   Data;

	struct Ref
	{
		Data data;
		int count;
		Handle handle;
	};

	typedef std::map<GxString, Handle> FileMap;
	typedef std::map<Handle, Ref> LoadMap;

	FileMap files;
	LoadMap loaded;
	const char* logTag;

	ResourceMap()
	{
		logTag = "ResourceMap";
	}

	~ResourceMap()
	{
	}

	void Insert(Handle handle, Data data, const char* path)
	{
		if(!files.insert(std::make_pair(path, handle)).second)
			GxLog(logTag, GX_LT_WARNING, "Failed to insert file \"%s\"", path);

		Insert(handle, data);
	}

	void Insert(Handle handle, Data data)
	{
		Ref ref;
		ref.count = 1;
		ref.data = data;
		ref.handle = handle;

		if(!loaded.insert(std::make_pair(handle, ref)).second)
			GxLog(logTag, GX_LT_WARNING, "Failed to insert handle %i", handle);
	}

	void AddReference(Handle handle)
	{
		if(handle)
		{
			LoadMap::iterator it = loaded.find(handle);
			if(it != loaded.end())
				++it->second.count;
			else
				GxLog(logTag, GX_LT_WARNING, "Adding reference to invalid handle %i", handle);
		}
	}

	Ref* GetData(const char* path)
	{
		FileMap::iterator itf = files.find(path);
		if(itf != files.end())
		{
			LoadMap::iterator it = loaded.find(itf->second);
			if(it != loaded.end())
				return &it->second;
		}
		return NULL;
	}

	Ref* GetData(Handle handle)
	{
		LoadMap::iterator it = loaded.find(handle);
		return (it == loaded.end()) ? NULL : &it->second;
	}

	Ref* Release(Handle handle)
	{
		if(!handle) return NULL;

		LoadMap::iterator it = loaded.find(handle);
		if(it != loaded.end())
		{
			--it->second.count;
			if(it->second.count == 0)
				return &it->second;
		}
		else
		{
			GxLog(logTag, GX_LT_WARNING, "Releasing invalid handle %i", handle);
		}
		return NULL;
	}

	bool Erase(Handle handle)
	{
		LoadMap::iterator it = loaded.find(handle);
		if(it == loaded.end())
		{
			GxLog(logTag, GX_LT_WARNING, "Erasing invalid handle %i", handle);
			return false;
		}
		loaded.erase(it);

		FileMap::const_iterator rem = files.begin();
		while(rem != files.end() && rem->second != handle)
			++rem;

		if(rem != files.end())
			files.erase(rem);

		return true;
	}

	int GetLoadedCount() const
	{
		return (int)loaded.size();
	}
};

}; // namespace core
}; // namespace guix