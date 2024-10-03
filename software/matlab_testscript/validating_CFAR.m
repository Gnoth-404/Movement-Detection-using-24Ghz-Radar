

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Testbench Communication from FreeSoc2 to Matlab
% Version 1.0, Nguyen Tien Anh Ha, 09.06.2024
%
% Behaviour: 
% - Everytime Maltlab writes �s�on the UART, the PSoC sends new measurement 
%    results and Matalab writes 'o' if these data is received.
% - The Script terminates after 10 data transfers.
%
% Using:
% 1. Connect FreeSoc2 to USB (i.e. Power Up)
% 2. Check the correct serial Port Settings
% 3. Start this Matlab Script
% 4. Run this Script
% 5. Press the external Push Button to start measuring
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

close all;
clear all;
clc;



priorPorts=instrfind;
delete(priorPorts);
PSoC=serial('/dev/ttyACM0', 'BaudRate', 115200, 'InputBufferSize', 14000);
fopen(PSoC);

f1 = figure;
count =1;

flg_data_avai = 0;
fwrite(PSoC,'s','uchar') % means send, I am ready to receive
while(flg_data_avai == 0)
       if PSoC.BytesAvailable == 1024*2+1024*4 + 1024*4
             fwrite(PSoC,'o','uchar') % means I received all expected data
             rx_data_adc = fread(PSoC,1024,'uint16');
             rx_data_power = fread(PSoC,1024,'int32');
             rx_data_threshold = fread(PSoC,1024,'int32');

             fprintf(" Transfer %i DONE \n",count);
             
             % Plotting the received data
                
             figure(f1)
             subplot(3,1,1)
             plot([0:(length(rx_data_adc)-1)],rx_data_adc(1:(length(rx_data_adc))));
             title(['Received Time Domain Data No.:',num2str(count)]);
             subplot(3,1,2)
             
             fft_matlab = fft(rx_data_adc);
             fft_scaling = (1/length(rx_data_adc))* abs(fft(rx_data_adc));
             fft_power = 20*log10(fft_scaling);

  



            cfar = phased.CFARDetector('NumTrainingCells',20,'NumGuardCells',2);
            exp_pfa = 1e-3;
            cfar.ThresholdFactor = 'Auto';
            cfar.ProbabilityFalseAlarm = exp_pfa;

            % Initialize count variable
            count2 = 0;
            
            % Increment count where power exceeds threshold
            for i = 1:length(rx_data_power)
                if rx_data_power(i) > rx_data_threshold(i) && rx_data_threshold(i) ~= 0                    
                    count2 = count2 + 1;
                end
            end
            
            % Display the count
            disp(['Count of rx\_data\_power > rx\_data\_threshold: ', num2str(count2)])


            subplot(3,1,2)
            plot([0:1023], rx_data_threshold, 'b', 'DisplayName', 'Threshhold - CFAR'); 
            hold on;
            %rx_data_power(rx_data_power == -2.147483648e+09) = 0;
            plot([0:1023], rx_data_power, 'r', 'DisplayName', 'Power - PSOC'); 

            % Add titles and labels
            %plot([0:1023], fft_power, 'r', 'DisplayName', 'Power - Matlab'); 

            title('FFT Comparison');
            xlabel('Frequency Bin');
            ylabel('Power (dB)');
            legend('show');

            hold off;



            %  % Save the received data
             save(strcat('CW_rx_data_adc_',int2str(count),'.mat'),'rx_data_adc');
             count=count+1;
       end

       if count == 11
           break;
       end

       fwrite(PSoC,'s','uchar') % means send, I am ready to receive
end

fclose(PSoC);

fprintf(" Scipt End \n");
