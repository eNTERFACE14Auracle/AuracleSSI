function ssi_anno_plot (anno, colors, handle)
%
% plot annotation
%
% input:
%   event                   events
%   colors                  line color
%   handle                  the axes handle (default: gca)
%
%
% by Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>
% created: 2007/11/28
% changed: 2007/11/28
% 

if nargin < 1
    help ssi_anno_plot
    error ('not enough arguments')
end

if nargin < 3 || isempty (handle)
    handle = gca;
end

axes (handle);
hold on;

annofields = fieldnames (anno);
nannos = length (annofields);
  
x_lims = get (handle, 'XLim');
y_lims = get (handle, 'YLim');
height = (y_lims(2)-y_lims(1)) / 50;
axis ([x_lims(1) x_lims(2) y_lims(1)-(nannos*height) y_lims(2)]);

for nanno = 1:nannos

    labels = anno.(annofields{nanno}).labels;
    lnames = anno.(annofields{nanno}).lnames;
    from = anno.(annofields{nanno}).from;
    to = anno.(annofields{nanno}).to;
    nlabels = length (labels);
    
    if nargin < 2 || isempty (handle)
        cs = defcolors (length (lnames));
    else
        cs = colors{nanno};
    end

    for nlabel = 1:nlabels
        line ([from(nlabel) to(nlabel)], [y_lims(1)-nanno*height y_lims(1)-nanno*height], 'Color', cs(labels(nlabel),:), 'LineWidth', height);
        text (from(nlabel) + (to(nlabel) - from(nlabel)) / 2, y_lims(1)-nanno*height, lnames{labels(nlabel)}, 'HorizontalAlignment', 'center', 'VerticalAlignment', 'bottom');    
    end
    
end