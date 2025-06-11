%This code snippet was authored using the GitHub Copilot AI programming assistant.
function heartbeat_gui
    % Create the GUI figure
    f = figure('Name','Synthetic Heartbeat Generator','Position',[300 300 400 320]);

    % BPM Slider
    uicontrol(f, 'Style','text','Position',[30 270 100 20],'String','BPM');
    bpmSlider = uicontrol(f, 'Style','slider','Min',40,'Max',180,'Value',60,...
        'Position',[130 270 200 20],'Callback',@updateLabels);
    bpmLabel = uicontrol(f, 'Style','text','Position',[340 270 40 20],'String','60');

    % Amplitude Slider
    uicontrol(f, 'Style','text','Position',[30 220 100 20],'String','Amplitude');
    ampSlider = uicontrol(f, 'Style','slider','Min',0,'Max',1,'Value',0.8,...
        'Position',[130 220 200 20],'Callback',@updateLabels);
    ampLabel = uicontrol(f, 'Style','text','Position',[340 220 40 20],'String','0.8');

    % Reverb Mix Slider
    uicontrol(f, 'Style','text','Position',[30 170 100 20],'String','Reverb Mix');
    reverbSlider = uicontrol(f, 'Style','slider','Min',0,'Max',1,'Value',0.3,...
        'Position',[130 170 200 20],'Callback',@updateLabels);
    reverbLabel = uicontrol(f, 'Style','text','Position',[340 170 40 20],'String','0.3');

    % Play Button
    uicontrol(f, 'Style','pushbutton','String','Play Heartbeat',...
        'Position',[50 100 120 40],'Callback',@playHeartbeat);

    % Save Button
    uicontrol(f, 'Style','pushbutton','String','Save to WAV',...
        'Position',[230 100 120 40],'Callback',@saveHeartbeat);

    % Update label values
    function updateLabels(~,~)
        bpmLabel.String = num2str(round(bpmSlider.Value));
        ampLabel.String = sprintf('%.2f', ampSlider.Value);
        reverbLabel.String = sprintf('%.2f', reverbSlider.Value);
    end

    % Generate heartbeat signal
    function y = generateHeartbeat(fs, duration, bpm, amplitude, wetMix)
        beat_interval = 60 / bpm;

        t_lub = 0:1/fs:0.12;
        t_dub = 0:1/fs:0.08;

        lub_tone = sin(2*pi*60*t_lub);
        lub_noise = randn(size(t_lub)) .* exp(-30*t_lub);
        lub = (lub_tone + 0.5*lub_noise) .* hann(length(t_lub))';

        dub_tone = sin(2*pi*90*t_dub);
        dub_noise = randn(size(t_dub)) .* exp(-40*t_dub);
        dub = (dub_tone + 0.4*dub_noise) .* hann(length(t_dub))';

        pause1 = zeros(1, round(0.1*fs));
        pause2 = zeros(1, round((beat_interval - 0.12 - 0.08 - 0.1)*fs));
        heartbeat = [lub pause1 dub pause2];

        num_beats = floor(duration / beat_interval);
        heartbeat_signal = repmat(heartbeat, 1, num_beats);

        lpFilt = designfilt('lowpassiir','FilterOrder',8, ...
            'PassbandFrequency',150,'PassbandRipple',0.2, ...
            'SampleRate',fs);
        heartbeat_filtered = filter(lpFilt, heartbeat_signal);

        reverb = reverberator( ...
            'PreDelay', 0.01, ...
            'WetDryMix', wetMix, ...
            'DecayFactor', 0.5, ...
            'SampleRate', fs);
        heartbeat_with_reverb = reverb(heartbeat_filtered(:)); % Ensure column vector

        y = amplitude * heartbeat_with_reverb;
    end

    % Play heartbeat
    function playHeartbeat(~,~)
        fs = 44100;
        duration = 10;
        bpm = round(bpmSlider.Value);
        amplitude = ampSlider.Value;
        wetMix = reverbSlider.Value;

        y = generateHeartbeat(fs, duration, bpm, amplitude, wetMix);
        sound(y, fs);

        % Plot waveform and spectrogram
        figure('Name','Heartbeat Visualization');
        subplot(2,1,1);
        plot((1:length(y))/fs, y);
        xlabel('Time (s)');
        ylabel('Amplitude');
        title('Waveform');

        subplot(2,1,2);
        spectrogram(y(:), 512, 256, 512, fs, 'yaxis'); % Ensure y is a vector
        title('Spectrogram');
    end

    % Save heartbeat to WAV
    function saveHeartbeat(~,~)
        fs = 44100;
        duration = 60;
        bpm = round(bpmSlider.Value);
        amplitude = ampSlider.Value;
        wetMix = reverbSlider.Value;

        y = generateHeartbeat(fs, duration, bpm, amplitude, wetMix);

        [file, path] = uiputfile('heartbeat.wav', 'Save Heartbeat As');
        if ischar(file)
            audiowrite(fullfile(path, file), y, fs);
            msgbox('Heartbeat saved successfully!', 'Success');
        end
    end
end
