lc = lcm.lcm.LCM.getSingleton();
aggregator = lcm.lcm.MessageAggregator();

lc.subscribe('HAUV_DIDSON_FRAME', aggregator);    % subscribe to didson stuff

close all;


while true
    %disp waiting
    millis_to_wait = 100;
    msg = aggregator.getNextMessage(millis_to_wait);
    if ~isempty(msg) > 0
        % got one!
        disp('received frame!');
        m = hauv.didson_t(msg.data);
        serializedImageData =typecast(m.m_cData, 'uint8');
        % deserialize (duh)
        frame = flip(reshape(serializedImageData, 96, 512)');
        
        % raw frame
        subplot(1,6,1);
        imshow(frame);
        title('Raw frame');
        
        % background
        subplot(1,6,2);
        background = imopen(imadjust(frame),strel('disk',15));
        surf(double(background(1:8:end,1:8:end))),zlim([0 255]);
        set(gca,'ydir','reverse');
        
        frame_2 = imadjust(imtophat(frame,strel('disk',15)));
        subplot(1,6,3)
        frame_bl = frame - background;
        imshow(imadjust(frame_bl));
        
        % contrast-enhanced
        subplot(1,6,4);
        imshow(frame_2);
        title('Contrast-enhanced');
        
        % binary
        subplot(1,6,5);
        level = graythresh(frame_2);
        bw = im2bw(frame_2,level);
        bw = bwareaopen(bw, 50);
        imshow(bw)

        % histogram
        subplot(1,6,6);
        [count, x] = imhist(frame);
        plot(x(128:end), count(128:end));
        ylim([0,100]);
        xlim([128,255]);
        disp(max(max(frame)));
        drawnow;
%         break;
    end
end

% frames are 96 beams per 512 bins!

% disp(sprintf('channel of received message: %s', char(msg.channel)))
% disp(sprintf('raw bytes of received message:'))
% disp(sprintf('%d ', msg.data'))
% 
% 
% disp(sprintf('decoded message:\n'))
