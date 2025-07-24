#pragma once
#include <JuceHeader.h>
#include "ComponentState.h"

class CloudServiceManager : public juce::Thread {
public:
    enum class ServiceType {
        None, Dropbox, GoogleDrive, OneDrive, Custom
    };

    struct CloudFile {
        juce::String id;
        juce::String name;
        juce::String path;
        juce::Time lastModified;
        int64 size;
        bool isFolder;
    };

    struct UserProfile {
        juce::String userId;
        juce::String username;
        juce::String email;
        ServiceType activeService;
        juce::String authToken;
    };

    CloudServiceManager();
    ~CloudServiceManager() override;

    bool authenticate(ServiceType service, const juce::String& credentials);
    void logout();
    bool isAuthenticated() const { return authenticated; }

    bool uploadFile(const juce::File& localFile, const juce::String& cloudPath);
    bool downloadFile(const juce::String& cloudPath, const juce::File& localFile);
    bool deleteFile(const juce::String& cloudPath);
    juce::Array<CloudFile> listFiles(const juce::String& path);

    bool sharePattern(const juce::String& patternData, const juce::String& metadata);
    bool shareKit(const juce::String& kitData, const juce::String& metadata);
    juce::Array<CloudFile> browseSharedContent(const juce::String& category);

    bool createSession(const juce::String& sessionName);
    bool joinSession(const juce::String& sessionId);
    void sendCollaborationData(const juce::String& data);

    void enableAutoSync(bool enable) { autoSyncEnabled = enable; }
    void syncNow();

    bool createSnapshot(const ComponentState& state, const juce::String& description);
    juce::Array<CloudFile> getSnapshots();
    bool restoreSnapshot(const juce::String& snapshotId, ComponentState& state);

    std::function<void(float)> onProgressUpdate;
    std::function<void(const juce::String&)> onError;
    std::function<void(const juce::String&)> onCollaborationDataReceived;

private:
    void run() override;

    bool authenticated = false;
    bool autoSyncEnabled = false;
    ServiceType currentService = ServiceType::None;
    UserProfile userProfile;

    std::unique_ptr<juce::URL::DownloadTask> currentTask;

    std::unique_ptr<juce::WebSocketClient> webSocket;

    juce::String getAPIEndpoint(const juce::String& operation);
    juce::String buildAuthHeader();

    juce::String serializeComponentState(const ComponentState& state);
    bool deserializeComponentState(const juce::String& data, ComponentState& state);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CloudServiceManager)
};
