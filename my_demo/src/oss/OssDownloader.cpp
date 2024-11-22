#include "../../inc/OssDownloader.h"
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>

OssDownloader::OssDownloader(const string& accessKeyId, const string& accessKeySecret, const string& endpoint) {
    ClientConfiguration config;
    client = new OssClient(endpoint, accessKeyId, accessKeySecret, config);
}

OssDownloader::~OssDownloader() {
    delete client;
}

void OssDownloader::downloadDirectory(const string& bucketName, const string& ossDir, const string& localDir) {
    listAndDownloadObjects(bucketName, ossDir, localDir);
}

void OssDownloader::listAndDownloadObjects(const string& bucketName, const string& ossDir, const string& localDir) {
    ListObjectsRequest request(bucketName);
    request.setPrefix(ossDir);  // 设置文件夹前缀，模拟文件夹
    auto outcome = client->ListObjects(request);

    if (outcome.isSuccess()) {
        const auto& objectSummaries = outcome.result().ObjectSummarys();
        
        for (const auto& objectSummary : objectSummaries) {
            string objectName = objectSummary.Key();
            string localFilePath = localDir + "/" + objectName.substr(ossDir.size() + 1);  // 本地文件路径

            string localDirPath = localFilePath.substr(0, localFilePath.find_last_of("/"));
            if (mkdir(localDirPath.c_str(), 0777) == -1 && errno != EEXIST) {
                cerr << "创建目录失败: " << localDirPath << endl;
                continue;
            }

            downloadFile(bucketName, objectName, localFilePath);
        }
    } else {
        cout << "列出对象失败: " << outcome.error().Message() << endl;
    }
}

void OssDownloader::downloadFile(const string& bucketName, const string& objectName, const string& localFilePath) {
    auto outcome = client->GetObject(bucketName, objectName, localFilePath);
    if (outcome.isSuccess()) {
        cout << "文件下载成功: " << objectName << endl;
    } else {
        cout << "文件下载失败: " << objectName << endl;
        cout << "错误信息: " << outcome.error().Message() << endl;
    }
}
