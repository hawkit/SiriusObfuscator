//===-- PlatformRemoteDarwinDevice.h -------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef liblldb_PlatformRemoteDarwinDevice_h_
#define liblldb_PlatformRemoteDarwinDevice_h_

// C Includes
// C++ Includes
#include <string>

// Other libraries and framework includes
// Project includes
#include "PlatformDarwin.h"
#include "lldb/Host/FileSpec.h"

#include "llvm/Support/FileSystem.h"

class PlatformRemoteDarwinDevice : public PlatformDarwin {
public:
  PlatformRemoteDarwinDevice();

  ~PlatformRemoteDarwinDevice() override;

  //------------------------------------------------------------
  // lldb_private::Platform functions
  //------------------------------------------------------------
  lldb_private::Error ResolveExecutable(
      const lldb_private::ModuleSpec &module_spec, lldb::ModuleSP &module_sp,
      const lldb_private::FileSpecList *module_search_paths_ptr) override;

  void GetStatus(lldb_private::Stream &strm) override;

  virtual lldb_private::Error
  GetSymbolFile(const lldb_private::FileSpec &platform_file,
                const lldb_private::UUID *uuid_ptr,
                lldb_private::FileSpec &local_file);

  lldb_private::Error
  GetSharedModule(const lldb_private::ModuleSpec &module_spec,
                  lldb_private::Process *process, lldb::ModuleSP &module_sp,
                  const lldb_private::FileSpecList *module_search_paths_ptr,
                  lldb::ModuleSP *old_module_sp_ptr,
                  bool *did_create_ptr) override;

  void
  AddClangModuleCompilationOptions(lldb_private::Target *target,
                                   std::vector<std::string> &options) override {
    return PlatformDarwin::AddClangModuleCompilationOptionsForSDKType(
        target, options, PlatformDarwin::SDKType::iPhoneOS);
  }

protected:
  static lldb_private::FileSpec::EnumerateDirectoryResult
  GetContainedFilesIntoVectorOfStringsCallback(
      void *baton, lldb_private::FileSpec::FileType file_type,
      const lldb_private::FileSpec &file_spec);

  struct SDKDirectoryInfo {
    SDKDirectoryInfo(const lldb_private::FileSpec &sdk_dir_spec);
    lldb_private::FileSpec directory;
    lldb_private::ConstString build;
    uint32_t version_major;
    uint32_t version_minor;
    uint32_t version_update;
    bool user_cached;
  };

  typedef std::vector<SDKDirectoryInfo> SDKDirectoryInfoCollection;

  std::mutex m_sdk_dir_mutex;
  SDKDirectoryInfoCollection m_sdk_directory_infos;
  std::string m_device_support_directory;
  std::string m_device_support_directory_for_os_version;
  std::string m_build_update;
  uint32_t m_last_module_sdk_idx;
  uint32_t m_connected_module_sdk_idx;

  bool UpdateSDKDirectoryInfosIfNeeded();

  const char *GetDeviceSupportDirectory();

  const char *GetDeviceSupportDirectoryForOSVersion();

  const SDKDirectoryInfo *GetSDKDirectoryForLatestOSVersion();

  const SDKDirectoryInfo *GetSDKDirectoryForCurrentOSVersion();

  static lldb_private::FileSpec::EnumerateDirectoryResult
  GetContainedFilesIntoVectorOfStringsCallback(
      void *baton, llvm::sys::fs::file_type ft,
      const lldb_private::FileSpec &file_spec);

  uint32_t FindFileInAllSDKs(const char *platform_file_path,
                             lldb_private::FileSpecList &file_list);

  bool GetFileInSDK(const char *platform_file_path, uint32_t sdk_idx,
                    lldb_private::FileSpec &local_file);

  uint32_t FindFileInAllSDKs(const lldb_private::FileSpec &platform_file,
                             lldb_private::FileSpecList &file_list);

  uint32_t GetConnectedSDKIndex();

  // Get index of SDK in SDKDirectoryInfoCollection by its pointer and return
  // UINT32_MAX if that SDK not found.
  uint32_t GetSDKIndexBySDKDirectoryInfo(const SDKDirectoryInfo *sdk_info);


  virtual void GetDeviceSupportDirectoryNames (std::vector<std::string> &dirnames) = 0;

  virtual std::string GetPlatformName () = 0;

private:
  DISALLOW_COPY_AND_ASSIGN(PlatformRemoteDarwinDevice);
};

#endif // liblldb_PlatformRemoteDarwinDevice_h_
