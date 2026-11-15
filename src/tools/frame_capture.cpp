#include "tools/frame_capture.h"

#include <ProjectUtils.h>
#include <atomic>
#include <cstring>
#include <vector>

namespace {
const int kIdle = 0;
const int kRecording = 1;
const int kDone = 2;

const int kGroups = 5;
const int kMaxFrames = 400;
const size_t kStampBytes = 4;

std::atomic<int> state{kIdle};
int group = 0;
int frames = 0;
int captured = 0;
std::vector<uint8_t> buffer;

const CRGB* groupLeds(const LED_MUSHROOMS_SET_t& leds, int index, int& count) {
    switch (index) {
    case 0:
        count = LED_NUM_BASS_1;
        return leds.leds_largeshrooms_left.leds;
    case 1:
        count = LED_NUM_BASS_2;
        return leds.leds_largeshrooms_right.leds;
    case 2:
        count = LED_NUM_MID;
        return leds.leds_whiteshrooms.leds;
    case 3:
        count = LED_NUM_TREBLE_1;
        return leds.leds_redshrooms_left.leds;
    default:
        count = LED_NUM_TREBLE_2;
        return leds.leds_redshrooms_right.leds;
    }
}

int groupSize(int index) {
    const int sizes[kGroups] = {LED_NUM_BASS_1, LED_NUM_BASS_2, LED_NUM_MID, LED_NUM_TREBLE_1,
                                LED_NUM_TREBLE_2};
    return sizes[index];
}

int intParam(AsyncWebServerRequest* request, const char* name, int fallback) {
    const AsyncWebParameter* param = request->getParam(name);
    return param != nullptr ? param->value().toInt() : fallback;
}

void sendStatus(AsyncWebServerRequest* request) {
    static const char* const kNames[] = {"idle", "recording", "done"};
    char json[128];
    snprintf(json, sizeof(json),
             "{\"state\":\"%s\",\"group\":%d,\"leds\":%d,\"frames\":%d,\"captured\":%d}",
             kNames[state.load()], group, groupSize(group), frames, captured);
    request->send(200, "application/json", json);
}
} // namespace

void frame_capture_init() {
    AsyncWebServer& server = ProjectUtils.server();
    // the longer paths first: a route also matches everything below it
    server.on("/debug/capture/start", HTTP_GET, [](AsyncWebServerRequest* request) {
        if (!ProjectUtils.web().authorize(request)) {
            return;
        }
        if (state.load() == kRecording) {
            request->send(409, "text/plain", "already recording");
            return;
        }
        const int wantedGroup = intParam(request, "group", 2);
        const int wantedFrames = intParam(request, "frames", 300);
        if (wantedGroup < 0 || wantedGroup >= kGroups || wantedFrames < 1 ||
            wantedFrames > kMaxFrames) {
            request->send(400, "text/plain", "group 0..4, frames 1..400");
            return;
        }
        group = wantedGroup;
        frames = wantedFrames;
        captured = 0;
        buffer.assign(static_cast<size_t>(frames) * (kStampBytes + groupSize(group) * 3), 0);
        state.store(kRecording); // the LED loop fills the buffer from now on
        sendStatus(request);
    });
    server.on("/debug/capture/status", HTTP_GET, [](AsyncWebServerRequest* request) {
        if (ProjectUtils.web().authorize(request)) {
            sendStatus(request);
        }
    });
    server.on("/debug/capture/data", HTTP_GET, [](AsyncWebServerRequest* request) {
        if (!ProjectUtils.web().authorize(request)) {
            return;
        }
        if (state.load() != kDone) {
            request->send(409, "text/plain", "no finished recording");
            return;
        }
        // the buffer stays untouched until the next start, so it can be sent without a copy
        request->send(200, "application/octet-stream", buffer.data(), buffer.size());
    });
}

void frame_capture_frame(const LED_MUSHROOMS_SET_t& leds) {
    if (state.load() != kRecording) {
        return;
    }
    int count = 0;
    const CRGB* colors = groupLeds(leds, group, count);
    uint8_t* out = buffer.data() + static_cast<size_t>(captured) * (kStampBytes + count * 3);
    const uint32_t stamp = micros();
    memcpy(out, &stamp, kStampBytes);
    memcpy(out + kStampBytes, colors, static_cast<size_t>(count) * 3);
    if (++captured >= frames) {
        state.store(kDone);
    }
}
