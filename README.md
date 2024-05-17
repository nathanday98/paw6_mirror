This is a source code mirror of my hobby project. It won’t build or run. This is mostly a playground for me to try different libraries or implement various tech. The code is probably quite messy and janky, as well as likely mid-refactor.

# Areas of interest
## Job Graph
The Job Graph is used to express work across a frame. You form a DAG of jobs, where each job is a function. It examines the parameters to the function, looks at the graph to find any job that could possibly conflict with you and checks if your parameters conflict. 
The design for the engine is to have two job graphs, one that executes at the display refresh rate and one that executes at a fixed rate.

Main code:\
https://github.com/nathanday98/paw6_mirror/blob/main/src/presentation/private/job_graph.h\
https://github.com/nathanday98/paw6_mirror/blob/main/src/presentation/private/job_graph.cpp\
Helper for analysing job parameters
https://github.com/nathanday98/paw6_mirror/blob/main/src/presentation/private/job_graph_add_helper.inl\
Usage code:
https://github.com/nathanday98/paw6_mirror/blob/main/src/presentation/private/simulation_graph.cpp\

## Render Graph
This is my take on a Render Graph. It’s incomplete currently. The idea is that you can define a graph of render passes using a visual layout tool and it will calculate the optimal memory layout / overlap and the needed barriers.

Main code:\
https://github.com/nathanday98/paw6_mirror/blob/3ccbd339534ac34e4ae99ead6ee7a5e384ad8236/src/presentation/private/renderer/dx12_renderer.cpp#L820\
Graph editor:\
https://github.com/nathanday98/paw6_mirror/blob/main/src/editor/private/main.cpp\

## Motion Matching
This is my first iteration at a motion matching implementation. It does some basic momentum prediction, performs a mostly brute force search of the motion-capture data and then uses a terrible inertialization blend. It mostly works, but there is still some sliding.

Anim DB:\
https://github.com/nathanday98/paw6_mirror/blob/main/src/presentation/private/motion_matching.h\
https://github.com/nathanday98/paw6_mirror/blob/main/src/presentation/private/motion_matching.cpp\

Character code:\
https://github.com/nathanday98/paw6_mirror/blob/main/src/presentation/private/game_state.cpp\

## USD
This is my investigation into using USD for pipeline and tools. I’m not completely sold, but it has made writing the graph layout quite trivial.

Graph Editor:\
https://github.com/nathanday98/paw6_mirror/blob/main/src/editor/private/main.cpp\

Anim loading:\
https://github.com/nathanday98/paw6_mirror/blob/main/src/presentation/private/usd_anim.cpp\

## Adobe Flash
This is my investigation into loading and rendering Adobe Flash files natively using loop-blinn rendering. 

https://github.com/nathanday98/paw6_mirror/blob/main/src/presentation/private/swf.cpp