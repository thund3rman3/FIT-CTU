    
%%% Read input images (drawing and scribbles):

pic = double(imread('drawing.png'));
usr = double(imread('scribbles.png'));

fig=1;     figure(fig); imagesc(pic/255); axis image; title('Drawing')
fig=fig+1; figure(fig); imagesc(usr/255); axis image; title('Scribbles')
pic = pic/255;

%%% Setting scale for edge capacities between two white pixels:

EK = 2000;

%%% Setting scale for edge capacities that correspond to soft scribbles:

SK = 200;

%%% Getting drawing dimensions:
h = size(pic, 1);
w = size(pic, 2);

%%% Setting indices of auxillary source "S" and sink "T" terminal vertices:

S = w * h + 1;
T = w * h + 2;

%%% Prepare an array "id" that assign unique index to each pixel (x,y) and 
%%% inverted arrays "idx" and "idy" that for the given vertex index returns 
%%% x and y coordiantes of its corresponding pixel [1.5 point]:


vid = 1; % vertex index
id = zeros(h,w);
idx = zeros(1, w);
idy = zeros(1, h);

for x=1:w
 for y=1:h
    id(y,x) = vid;
    idx(vid) = x;
    idy(vid) = y;
    vid = vid + 1; 
 end
end

%%% Setup directed edges going from the source terminal "S" to all pixels 
%%% and from all pixels to the sink terminal "T" and set their capacities
%%% according to scribbles stored in "usr" (use "SK") [3 points]:

eid = 1; % edge index
edges = 4*w*h - 2*w - 2*h + 2*w*h;
source = zeros(1, edges);
target = zeros(1, edges);
capacity = zeros(1, edges);

for x=1:w
 for y=1:h
  %% Graph edges are specified using three index arrays:
  %%
  %% "source(eid)" = index of source (from) graph vertex
  %% "target(eid)" = index of target (to) graph vertex
  %% "capacity(eid)" = edge capacity
    source(eid) = S;
    target(eid) = id(y,x);
    if usr(y,x,1) == 255
        capacity(eid) = SK;
    end
    eid = eid + 1;
    
    source(eid) = id(y,x);
    target(eid) = T;
    if usr(y,x,1) == 14
        capacity(eid) = SK;
    end
    eid = eid + 1;
 end 
end


%%% Setup capacities of undirected edges between neighbour pixels 
%%% (undirected edge = two directed edges in opposite direction with 
%%% the same capacity). As a capacity use "EK" scaled in proportion to 
%%% pixel's intensity. You can use gamma correction to improve contrast 
%%% between white and dark pixels (remember that edge capacity between 
%%% pixels shouldn't be equal to zero) [4 points]:

gamma = 6;

for x=1:w
 for y=1:h
     if y < h
        capY = 1 + EK * min(pic(y,x), pic(y+1,x))^gamma;
     end
     if x < w
        capX = 1 + EK * min(pic(y,x), pic(y,x+1))^gamma;
     end

     if x < w && y < h
         source(eid:eid+3) = [id(y,x),id(y+1,x),id(y,x),id(y,x+1)];
         target(eid:eid+3) = [id(y+1,x),id(y,x),id(y,x+1),id(y,x)];
         capacity(eid:eid+3) = [capY,capY,capX,capX];
         eid = eid + 4;

     elseif y == h && x < w
         source(eid:eid+1) = [id(y,x+1),id(y,x)];
         target(eid:eid+1) = [id(y,x),id(y,x+1)];
         capacity(eid:eid+1) = [capX,capX];
         eid = eid + 2;

     elseif x == w && y < h
         source(eid:eid+1) = [id(y+1,x),id(y,x)];
         target(eid:eid+1) = [id(y,x),id(y+1,x)];
         capacity(eid:eid+1) = [capY,capY];
         eid = eid + 2;
     end
 end 
end

%%% Construct a directed graph "G" and solve for maximum flow between 
%%% source "S" and sink "T" terminal vertices. Use "source", "target", 
%%% and "capacity" arrays prepared in previous steps:

G = digraph(source, target, capacity);

[MF,GF,CS,CT] = maxflow(G,S,T);

%%% Get indices of pixels labelled as belonging to terminal "T" stored 
%%% in the array "CT" and colorize them by multiplying their original 
%%% intensity with the color components of the green color. Use "idx"
%%% and "idy" to lookup pixel coordiantes [1.5 points]: 

out = pic*255; % copy the input drawing to the output

col(1) = 14; % setup green color RGB values
col(2) = 138;
col(3) = 8; 

for id=1:size(CT, 1)-1
 y = idy(CT(id)); 
 x = idx(CT(id));
 out(y,x,1) = pic(y,x,1) * col(1); 
 out(y,x,2) = pic(y,x,2) * col(2);
 out(y,x,3) = pic(y,x,3) * col(3);
end

%%% Show the resulting colorized image:

fig=fig+1; figure(fig); imagesc(out/255); axis image; title('Output')
