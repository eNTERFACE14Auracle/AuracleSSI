function ssi_signal_write (signal, sr, path, s_type)
%
% write signal to file
%
% ssi_signal_write (signal, sr, path, [type])
%
% input:
%   signal                  signal
%   sr                      sample rate
%   path                    path
%   type                    type string
%   ([U]CHAR,[U]SHORT,[U]INT,[U]LONG,FLOAT,DOUBLE])
%
%
% by Johannes Wagner <johannes.wagner@informatik.uni-augsburg.de>
% created: 2014/05/05

if nargin < 3
    help ssi_signal_write;
    error ('not enough arguments')
end

if nargin < 4
    s_type = 'FLOAT';
end

s_byte = 0;
switch (s_type)
    case 'CHAR'
        type = 'int8';        
        s_byte = 1;
    case 'UCHAR'
        type = 'uint8';        
        s_byte = 1;
    case 'SHORT'
        type = 'int16';        
        s_byte = 2;
    case 'USHORT'
        type = 'uint16';        
        s_byte = 2;
    case 'INT'
        type = 'int32';        
        s_byte = 4;
    case 'UINT'
        type = 'uint32';        
        s_byte = 4;
    case 'LONG'
        type = 'int64';        
        s_byte = 8;
    case 'ULONG'
        type = 'uint64';        
        s_byte = 8;
    case 'FLOAT'
        type = 'single';    
        s_byte = 4;
    case 'DOUBLE'
        type = 'double';        
        s_byte = 8;
    otherwise
        error (['type ' type_s ' not supported']);
end

[pathstr, name, ext] = fileparts (path);
path_data = [pathstr name '.stream~'];
if isempty (ext)
    path_info = [path '.stream'];
else
    path_info = path;
end

[num, dim] = size (signal);

docNode = com.mathworks.xml.XMLUtils.createDocument ('stream');
docRootNode = docNode.getDocumentElement;
docRootNode.setAttribute ('ssi-v','2');

docElement = docNode.createElement ('info'); 
docElement.setAttribute ('ftype','BINARY');
docElement.setAttribute ('sr', num2str(sr));
docElement.setAttribute ('dim', num2str(dim));
docElement.setAttribute ('byte', num2str(s_byte));
docElement.setAttribute ('type', s_type);
docRootNode.appendChild(docElement);

docElement = docNode.createElement ('chunk'); 
docElement.setAttribute ('from', '0');
docElement.setAttribute ('to', num2str(num/sr));
docElement.setAttribute ('byte', '0');
docElement.setAttribute ('num', num2str(num));
docRootNode.appendChild(docElement);

xmlwrite (path_info, docNode);

fid = fopen (path_data, 'w');
fwrite (fid, signal', type);
fclose (fid);