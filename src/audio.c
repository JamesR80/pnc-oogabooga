	// Allocator audioHeap = get_heap_allocator();


    
    // {
    //     for (int i = 0; i < a_MAX; i++)
    //     {
    //         sounds[i] = 
    //     }
    // }
    
    // Audio_Source train, song1, song2, song3, song4, song5, song6, song7, sound1, sound2, sound3, sound4;

    void loadAudio(AudioID audioID, string path)
    {
        
        Audio_Source src;
        
        if (audioID == 0) // add missing audio sound
        {
            // ZERO(src);
            // ZERO(*player);
        } 
        else if (audioID > 0 && audioID < 9) // songs and background noise
        {
            bool songOK = audio_open_source_stream(&src, path, get_heap_allocator());
            assert(songOK, "Could not load audioID %i", audioID);
        }
        else if (audioID > 8 && audioID < au_MAX)
        {
            bool soundOK = audio_open_source_stream(&src, path, get_heap_allocator());
            assert(soundOK, "Could not load audioID %i", audioID);
        }

        world->audioSources[audioID] = src;        
    }

    Audio_Source* getAudioSource(AudioID audioID)
    {
        if (audioID >= 0 && audioID < au_MAX) return &world->audioSources[audioID];
        else return &world->audioSources[0];
    }

    // Audio_Player* getAudioPlayer(AudioID audioID)
    // {
    //     if (audioID >= 0 && audioID < au_MAX) return &world->audioPlayers[audioID];
    //     else return &world->audioPlayers[0];
    // }
	
    AudioID getAudioIDFromBG(Entity* background)
    {
        if (background->type == t_background)
        {
            switch (background->spriteID)
            {
                case s_bg_cargo:
                    return au_song2;
                case s_bg_dining:
                    return au_song3;
                case s_bg_luggage:
                    return au_song4;
                case s_bg_lounge:
                    return au_song5;
                case s_bg_hallway:
                    return au_song6;
                case s_bg_sleeper:
                    return au_song7;
                case s_bg_menu:
                    return au_song1;
                case s_bg_settings:
                    return au_song1;
                default:
                    return au_nil;
            }
        }
        else return au_nil;
    }



	// bool sound1_ok = audio_open_source_load(&sound1, STR("oogabooga/examples/bruh.wav"), audioHeap);
	// assert(sound1_ok, "Could not load bruh.wav");

	// Audio_Player* clipPlayer = audio_player_get_one();
	// Audio_Player* song1Player = audio_player_get_one();
	// Audio_Player* song2Player = audio_player_get_one();
	// Audio_Player* song3Player = audio_player_get_one();
	// Audio_Player* song4Player = audio_player_get_one();
	// Audio_Player* song5Player = audio_player_get_one();
	// Audio_Player* song6Player = audio_player_get_one();
	// Audio_Player* song7Player = audio_player_get_one();

	// Audio_Player* trainPlayer = audio_player_get_one();

	// audio_player_set_source(clipPlayer, bruh);
    
	// audio_player_set_source(song1Player, song1);
	// audio_player_set_source(song2Player, song2);
	// audio_player_set_source(song3Player, song3);
	// audio_player_set_source(song4Player, song4);
	// audio_player_set_source(song5Player, song5);
	// audio_player_set_source(song6Player, song6);
	// audio_player_set_source(song7Player, song7);
	// audio_player_set_source(trainPlayer, train);
	
	// audio_player_set_state(clipPlayer, AUDIO_PLAYER_STATE_PAUSED);
	// audio_player_set_state(song1Player, AUDIO_PLAYER_STATE_PAUSED);
	// audio_player_set_state(song2Player, AUDIO_PLAYER_STATE_PAUSED);
	// audio_player_set_state(song3Player, AUDIO_PLAYER_STATE_PAUSED);
	// audio_player_set_state(song4Player, AUDIO_PLAYER_STATE_PAUSED);
	// audio_player_set_state(song5Player, AUDIO_PLAYER_STATE_PAUSED);
	// audio_player_set_state(song6Player, AUDIO_PLAYER_STATE_PAUSED);
	// audio_player_set_state(song7Player, AUDIO_PLAYER_STATE_PAUSED);
	// audio_player_set_state(trainPlayer, AUDIO_PLAYER_STATE_PAUSED);


	// audio_player_set_looping(song1Player, true);
	// audio_player_set_looping(song2Player, true);
	// audio_player_set_looping(song3Player, true);
	// audio_player_set_looping(song4Player, true);
	// audio_player_set_looping(song5Player, true);
	// audio_player_set_looping(song6Player, true);
	// audio_player_set_looping(song7Player, true);
	// audio_player_set_looping(trainPlayer, true);

	// audio_player_set_looping(clipPlayer, false);

	// bool clipPlaying = clipPlayer->state == AUDIO_PLAYER_STATE_PLAYING;
	// bool song1Playing = song1Player->state == AUDIO_PLAYER_STATE_PLAYING;
	// bool song2Playing = song2Player->state == AUDIO_PLAYER_STATE_PLAYING;
	// bool song3Playing = song3Player->state == AUDIO_PLAYER_STATE_PLAYING;
	// bool song4Playing = song4Player->state == AUDIO_PLAYER_STATE_PLAYING;
	// bool song5Playing = song5Player->state == AUDIO_PLAYER_STATE_PLAYING;
	// bool song6Playing = song6Player->state == AUDIO_PLAYER_STATE_PLAYING;
	// bool song7Playing = song7Player->state == AUDIO_PLAYER_STATE_PLAYING;
	// bool trainPlaying = trainPlayer->state == AUDIO_PLAYER_STATE_PLAYING;

