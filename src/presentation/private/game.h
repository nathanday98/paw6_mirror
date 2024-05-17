#pragma once

#include <shared/std.h>

struct Platform;
struct PlatformEvent;
struct UTF8StringView;

struct GameStartupArgs;

struct GameSystems;

GameSystems* game_init(Platform& platform, const GameStartupArgs& startup_args, void* graphics_adapter, void* graphics_factory);
void game_deinit(GameSystems* self, Platform& platform);