#ifndef OSS_DOWNLOADER_H
#define OSS_DOWNLOADER_H

#include <alibabacloud/oss/OssClient.h>
#include <alibabacloud/oss/OssError.h>
#include <alibabacloud/oss/ServiceRequest.h>
#include <string>

using namespace AlibabaCloud::OSS;
using namespace std;
class OssDownloader {

  private:
    // OSS 客户端
    OssClient * client;
    // 下载单个文件
    void downloadFile(const string & bucketName, const string & objectName, const string & localFilePath);

    // 列出 OSS 文件并下载
    void listAndDownloadObjects(const string & bucketName, const string & ossDir, const string & localDir);

  public:
    OssDownloader(const string & accessKeyId, const string & accessKeySecret, const string & endpoint);
    ~OssDownloader();

    // 下载文件夹中的文件到本地
    void downloadDirectory(const string & bucketName, const string & ossDir, const string & localDir);
};

#endif // OSS_DOWNLOADER_H
