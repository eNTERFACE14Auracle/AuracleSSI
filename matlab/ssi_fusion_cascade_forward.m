function [cascade_probs, cascade_votes] = ssi_fusion_cascade_forward (cascade_model, samples, dims)
% Fusion strategy 'cascading specialist'
%
%  probs = ssi_fusion_cascade_forward (model, samples, dims)
%   
%   input:
%   model:    fusion model
%   samples:  samples (one sample per row)
%   dims:     channel range ( nth channel: samples(:,dims(n)+1:dims(n+1)) )
%
%   output:
%   probs:    a matrix with the probability errors for each sample compared to each
%             class (the row indicates the sample and the column the class)
%   votes:    vector with winner classes
%
%   2010, Johannes Wagner <go.joe@gmx.de>

clen = cascade_model.nclass;
slen = size (samples, 1);

models = cascade_model.models;
winner = cascade_model.winner;
order = cascade_model.order;
filler = cascade_model.filler;

cascade_probs = zeros (slen, clen);
cascade_votes = zeros (slen, 1);

for sn = 1:slen
          
    %CLASSIFIER CASCADE
             
    for k = 1:clen

        next = order(k);               
        model = models(winner(next));        
        channel = ssi_fusion_getchannel (samples, dims, winner(next));
        
        [probs, vote] = cascade_model.fforward (model, channel(sn,:));
        
        if vote == next
            cascade_votes(sn) = next;
            cascade_probs(sn,:) = probs;
            break;
        end
            
    end %for k = 1:csize ...
    
    %kein Klassifikator hat die ihm zugewiesene Klasse erkannt -> der Fill
    %Klassifikator entscheidet
    if ( (cascade_votes(sn) == 0) )
      
        model = models(filler);        
        channel = get_channel (samples, dims, filler);
        
        [probs, vote] = cascade_model.fforward (model, channel(sn,:));        
       
        cascade_votes(sn) = vote;
        cascade_probs(sn,:) = probs;
                        
    end %if ...
    
end %for i = 1:ssize
