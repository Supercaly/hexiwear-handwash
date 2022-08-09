function d = subsample(data)
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here
    d0 = data(data.label==0,:);
    d1 = data(data.label==1,:);
    d2 = data(data.label==2,:);

    min_size = min(size(d0,1), min(size(d1,1), size(d2,1)));

    d0 = d0(randperm(size(d0,1)),:);
    d1 = d1(randperm(size(d1,1)),:);
    d2 = d2(randperm(size(d2,1)),:);
    d0 = d0(randperm(size(d0,1)),:);
    d1 = d1(randperm(size(d1,1)),:);
    d2 = d2(randperm(size(d2,1)),:);
    d0 = d0(randperm(size(d0,1)),:);
    d1 = d1(randperm(size(d1,1)),:);
    d2 = d2(randperm(size(d2,1)),:);

    d0 = d0(1:min_size,:);
    d1 = d1(1:min_size,:);
    d2 = d2(1:min_size,:);

    d = [d0;d1;d2];
    d = d(randperm(size(d,1)),:);
    d = d(randperm(size(d,1)),:);
    d = d(randperm(size(d,1)),:);
end