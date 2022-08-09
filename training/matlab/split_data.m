
function r = split_data(t, r, l)
    idx = 1;
    
    for i = 1:200:1629000
%         r(idx,:,:) = table2array(t(i:199+i, 1:end-1));
        a = table2array(t(i:199+i, 1:end-1))';
        r(idx,:) = a(:);
        l(idx) = mode(table2array(t(i:199+i, end)));
        idx = idx +1;
    end
end