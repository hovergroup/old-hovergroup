lc = lcm.lcm.LCM.getSingleton();
aggregator = lcm.lcm.MessageAggregator();

lc.subscribe('HAUV_DIDSON_FRAME', aggregator);    % subscribe to didson stuff

while true
    disp waiting
    millis_to_wait = 100;
    msg = aggregator.getNextMessage(millis_to_wait);
    if ~isempty(msg) > 0
        % got one!
        m = hauv.didson_t(msg.data);
        img = reshape(m.m_cData, 96, 512);
        imshow(img);
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
