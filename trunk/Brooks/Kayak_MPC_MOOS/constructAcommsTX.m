function [send,frames] = constructAcommsTX(acommsRate,data)
% creates an ASCII string to send over acomms
% function [send,frames] = constructAcommsTX(acommsRate,data)
% send: string to send
% frames: each frame string 
% acommsRate: {0,1,2,3,4,5,100}
% data: if STRING: packs, pads, (warning if too large)
% data: if CELL of STRINGS: packs each string into frames (warnings if too
% large or too many)
%
% numFrames/frameSize:
% 1: 1/32, 2: 3/64, 3: 2/256, 4: 2/256, 5: 8/256, 100: 1/13 BITS

% BR, 11/12/2012

% (later make binary option?)

switch acommsRate
    case 0
        numFrames = 1;
        frameSize = 32;
        
    case 1
        numFrames = 3;
        frameSize = 64;
        
    case 2
        numFrames = 3;
        frameSize = 64;
        
    case 3
        numFrames = 2;
        frameSize = 256;
        
    case 4
        numFrames = 2;
        frameSize = 256;
        
    case 5
        numFrames = 8;
        frameSize = 256;
        
    case 100
        numFrames = 1;
        frameSize = 13; %BITS
        
        %%% THIS WORKS ONLY WITH BINARY DATA...
        
end
packetLen = numFrames*frameSize;

if(ischar(data))
    dataLen = length(data);
    if(dataLen>packetLen)
        disp('WARNING: TOO MUCH DATA, TRUNCATING')
        data = data(1:packetLen);
    end
    pad = packetLen - dataLen;
    padstr = sprintf('%s',repmat('0',pad,1));
    send  = sprintf('%s%s',data,padstr);
    frames = {send};
end

if(iscell(data))
    
    dataFrames = length(data);
    if(dataFrames>numFrames)
        disp('WARNING: TOO MANY DATA FRAMES, TRUNCATING')
        data = data(1:numFrames);
    elseif(dataFrames<numFrames)
        disp('WARNING: PADDING DATA FRAMES')
        missingFrames = numFrames - dataFrames;
        for i = 1:missingFrames
            data{numFrames-(i-1)} = sprintf('%s',repmat('0',frameSize,1));
        end
    end
    
    send = '';
    for i = 1:numFrames
        frameData = data{i};
        dataLen = length(frameData);
        if(dataLen>frameSize)
            disp('WARNING: TOO MUCH DATA, TRUNCATING')
            frameData = frameData(1:frameSize);
        end
        pad = frameSize - dataLen;
        padstr = sprintf('%s',repmat('0',pad,1));
        frames{i} = sprintf('%s%s',frameData,padstr);
        send  = [send frames{i}];

    end
    
end

