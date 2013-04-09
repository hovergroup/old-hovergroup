% iMatlab test
% 8/13/2012
% BR


clear all;
close all;
clc;


%addpath('/home/mei/moos-ivp/MOOS/MOOSBin/')

moosDB = 'icarus.moos';
%pathName = '/home/mei/hovergroup/ivp-extend/mei/missions/mei_relay/';
pathName = '/home/brooks/hovergroup/ivp-extend/brooks/missions';


%moosDB = 'iMatlab.moos';
%pathName = '/home/mei/moos-ivp/MOOS/Tools/Matlab/iMatlab/';

old = cd(pathName)
iMatlab('init','CONFIG_FILE',moosDB);


%configName = sprintf('%s%s',pathName,moosDB);
%iMatlab('init','CONFIG_FILE',configName,'MOOSNAME','iMatlab_test')
%iMatlab('init')




%%
%iMatlab('MOOS_PAUSE',0,3);
pause(3)

for i = 1:3

   
    
    
    %Extract message content
    mail{i}=iMatlab('MOOS_MAIL_RX');
    messages=length(mail{i});
% 
%     if messages==0
%         continue;
%     end
    
%     % Process messages
%     for m=1:messages
% 
%         msg_tic = msg_tic+1
% 
%         key=mail(m).KEY;
%         val=mail(m).DBL;
%         str=mail(m).STR;
% 
% 
%         switch key
% 
%             case 
%         end
%       end

    % break out of loop?
    
    % post something
    sendVar = 'IMATLAB_TEST_GARBAGE';
    sendVal = 10+i;
    
    
    iMatlab('MOOS_MAIL_TX',sendVar,sendVal)
    fprintf('\nSending: %s = %i \n',sendVar,sendVal)
    
    pause(2)
    
    %iMatlab('MOOS_PAUSE',0,5)
    
end

cd(old)