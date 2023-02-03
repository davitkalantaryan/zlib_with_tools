# zlib with tools  

This repository is to work with [zlib](https://github.com/madler/zlib) and implements some tools 
based on the [zlib](https://github.com/madler/zlib).

## How to build  
  
If you are a developer then in order to compile ZLIB and tools following steps should be confirmed (currently only for Windows)  
  
```bat
git clone https://github.com/davitkalantaryan/zlib_with_tools.git  :: or one can fork/copy/clone and use another remote  
cd zlib_with_tools  :: change the directory to the repository root  
.\scripts\windows_prepare_host.bat   :: Initialize developer host (skip this if you did this once)  
.\scripts\windows_prepare_repository.bat   ::  Initialize repository (submodules and some config files)  
:: for the moment building ZLIB and tools should be done from IDE  
:: open workspaces\zlib_with_tools_all_vs\zlib_with_tools_all.sln file using Visual Studio and build all  
```  
