500882444 Taehyun Ahn tahn@ryerson.ca
Player vs AI battle bot simulation. User can switch between first person camera and bird eye view camera.
OpenGL matrix calculations and texture mapping are used for the project through c++.

![alt text](https://github.com/athkev/battleBots/blob/main/sample.gif)

Controls:
	F1: Help
	W: Forward
	A: Turn Left
	S: Backward
	D: Turn Right

	Spacebar: rotate blade
	f: stop rotating blade
	v: toggle first person camera

	e: rotate scene
	E: stop rotating scene
	b: toggle AI movements (AI will stop/move)
	

Compiling:
	open a2fix.sln through visual studio,
	compile and run Robot3dUpdated.cpp through Local Windows Debugger (Visual Studio) in x86!!!

*initially, player must press spacebar inorder to spin the blade and attack. Otherwise player will only be able to do contact damage.

both player and AI will take 1 to 2 random damage when in contact each other (circle collision)
if player's blade is rotating and hits AI's body, 1 to 5 random damage each tick.
if AI's blade hits player's body, 1 to 5 random damage each tick.

press e to rotate scene incase robot goes out of scene.
