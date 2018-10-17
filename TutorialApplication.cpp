//
// Student Name:
// Student ID:
// Student Email Address:
//
//
// 3D Game Programming
// NCTU
// Instructor: SAI-KEUNG WONG
//
//
// Date: 2018/10/08
//
#include "TutorialApplication.h"
#include "BasicTools.h"
using namespace Ogre;
using namespace std;
BasicTutorial_00::BasicTutorial_00(void) {
	mSound = new SOUND;
	mSound->init();

	lightAngle = 0;
	lightRotateSpeed = 30;
	lightRadius = 1000.0;

	isPress = false;
	isMoving = false;
}

void BasicTutorial_00::createCamera(void)
{
	mCamera = mSceneMgr->createCamera("PlayerCamera");

	mCamera->setPosition(Ogre::Vector3(0,700,800));
	mCamera->lookAt(Ogre::Vector3(0,0,0));

	mCamera->setNearClipDistance(5);

	mCameraMan = new OgreBites::SdkCameraMan(mCamera);   // create a default camera controller
}

void BasicTutorial_00::createViewports(void)
{
	Ogre::Viewport* vp;
	// Create one viewport, entire window
	vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio(
		Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}

void BasicTutorial_00::createScene(void) 
{
	mSceneMgr->setAmbientLight( ColourValue( 0.9, 0.9, 0.9 ) ); 
	mSceneMgr->setFog(Ogre::FOG_LINEAR, Ogre::ColourValue(1, 1, 1), 0, 1400, 1600);
	mSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox");
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

	// Selection Box
	mSelectRect = new SelectionRectangle("SelectionRectangle");
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(mSelectRect);

	mVolQuery = mSceneMgr->createPlaneBoundedVolumeQuery(Ogre::PlaneBoundedVolumeList());

	// Light
	Light *light = mSceneMgr->createLight("Light");
	light->setCastShadows(true);
	light->setDiffuseColour(1, 1, 1);
	light->setType(Ogre::Light::LT_POINT);
	light->setPosition(1000, 500, 0);
	
	// plane
	Plane plane(Vector3::UNIT_Y, 0); 
	MeshManager::getSingleton().createPlane("ground", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		plane, 1500, 1500, 
		20, 20, 
		true, 
		1, 
		5, 5, 
		Ogre::Vector3::UNIT_Z);
	Entity *ent = mSceneMgr->createEntity("GroundEntity", "ground");
	ent->setMaterialName("Examples/Rocky");
	mSceneMgr->getRootSceneNode()->createChildSceneNode("PlaneNode", Ogre::Vector3(0, 0, 0))->attachObject(ent);

	// Robot
	int robot_num = 36;
	float radius = 200;
	float radian = 3.1415 / 180.0;
	float step = 360.0 / robot_num;

	for (int i=0; i<robot_num; i++) {
		Ogre::String ent_name;
		stringCreate("inner_robot_", i, ent_name);
		std::cout << cos(radian * step * i) << " " << sin(radian * step * i) << std::endl;

		ent = mSceneMgr->createEntity(ent_name, "robot.mesh");
		SceneNode *snode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		snode->attachObject(ent);
		snode->setPosition(radius * cos(radian * step * i), 0, radius * sin(radian * step * i));
		snode->lookAt(Ogre::Vector3(0, 0, 0), Ogre::Node::TransformSpace::TS_WORLD, Ogre::Vector3::UNIT_Z);
		snode->rotate(Ogre::Quaternion(sqrt(0.5), 0, -sqrt(0.5), 0));
	}

	radius = 300;
	for (int i=0; i<robot_num; i++) {
		Ogre::String ent_name;
		stringCreate("outer_robot_", i, ent_name);
		std::cout << cos(radian * step * i) << " " << sin(radian * step * i) << std::endl;

		ent = mSceneMgr->createEntity(ent_name, "robot.mesh");
		SceneNode *snode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		snode->attachObject(ent);
		snode->setPosition(radius * cos(radian * step * i), 0, radius * sin(radian * step * i));
		snode->lookAt(Ogre::Vector3(0, 0, 0), Ogre::Node::TransformSpace::TS_WORLD, Ogre::Vector3::UNIT_Z);
		snode->rotate(Ogre::Quaternion(sqrt(0.5), 0, -sqrt(0.5), 0));
		if (!i) {
			snode->scale(2, 2, 2);
		}
	}

	// Sphere
	ent = mSceneMgr->createEntity("center_sphere", "sphere.mesh");
	SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode("SphereNode", Ogre::Vector3::ZERO);
	node->attachObject(ent);
	Ogre::AxisAlignedBox bb = ent->getBoundingBox();
	Ogre::Real size = bb.getMaximum().x - bb.getMinimum().x;
	size = 140.0 / size;
	node->scale(size, size, size);
}
bool BasicTutorial_00::mouseMoved( const OIS::MouseEvent &arg )
{
	if (isPress) {
		isMoving = true;
		Ogre::Ray mRay = mTrayMgr->getCursorRay(mCamera);
		endPoint = mTrayMgr->sceneToScreen(mCamera, mRay.getOrigin());

		mSelectRect->setVisible(true);
		mSelectRect->setCorners(min(startPoint.x, endPoint.x), min(startPoint.y, endPoint.y), max(startPoint.x, endPoint.x), max(startPoint.y, endPoint.y));
	}
	return BaseApplication::mouseMoved( arg);
}

//void BasicTutorial_00::singleClickSelect ( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
//{
//}

void BasicTutorial_00::volumeSelect()
{
	float left = min(startPoint.x, endPoint.x), 
		top = min(startPoint.y, endPoint.y), 
		right = max(startPoint.x, endPoint.x), 
		bottom = max(startPoint.y, endPoint.y);
	Ray topLeft = mCamera->getCameraToViewportRay(left, top);
	Ray topRight = mCamera->getCameraToViewportRay(right, top);
	Ray bottomLeft = mCamera->getCameraToViewportRay(left, bottom);
	Ray bottomRight = mCamera->getCameraToViewportRay(right, bottom);
	// The plane faces the counter clockwise position.
	PlaneBoundedVolume vol;
	int np = 100;
	vol.planes.push_back(Plane(topLeft.getPoint(3), topRight.getPoint(3), 			bottomRight.getPoint(3)));         // front plane
	vol.planes.push_back(Plane(topLeft.getOrigin(), topLeft.getPoint(np), 	topRight.getPoint(np)));         // top plane
	vol.planes.push_back(Plane(topLeft.getOrigin(), bottomLeft.getPoint(np), 	topLeft.getPoint(np)));       // left plane
	vol.planes.push_back(Plane(bottomLeft.getOrigin(), bottomRight.getPoint(np), 	bottomLeft.getPoint(np)));   // bottom plane
	vol.planes.push_back(Plane(bottomRight.getOrigin(), topRight.getPoint(np), 	bottomRight.getPoint(np)));     // right plane 

}

bool BasicTutorial_00::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if (!isMoving) {
		//std::cout << "Is Single Press\n";
	}

	isMoving = isPress = false;
	mSelectRect->setVisible(false);
	return BaseApplication::mouseReleased( arg, id );
}

bool BasicTutorial_00::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if (id == OIS::MB_Right) {
		return BaseApplication::mousePressed( arg, id );
	}

	isPress = true;
	
	Ogre::Ray mRay = mTrayMgr->getCursorRay(mCamera);
	startPoint = mTrayMgr->sceneToScreen(mCamera, mRay.getOrigin());

	//std::cout << startPoint.x << " " << startPoint.y << std::endl;
	return BaseApplication::mousePressed( arg, id );
}

bool BasicTutorial_00::frameStarted(const FrameEvent &evt)
{
	// Rotate Light
	float radian = 3.1415 / 180.0;
	Light *light = mSceneMgr->getLight("Light");
	lightAngle += lightRotateSpeed * evt.timeSinceLastFrame;
	light->setPosition(lightRadius * cos(radian * lightAngle), 500, lightRadius * sin(radian * lightAngle));
	//std::cout << Ogre::Vector3(lightRadius * cos(radian * lightAngle), 500, lightRadius * sin(radian * lightAngle)) << std::endl;

    return BaseApplication::frameStarted(evt);
}

void BasicTutorial_00::stringCreate(const Ogre::String & prefix, int index, Ogre::String &out_name)
{
	out_name= prefix + Ogre::StringConverter::toString(static_cast<int>(index));
}

void BasicTutorial_00::initParticleSystemForExplosion() {
	mParticleNode = static_cast<SceneNode*>(
		mSceneMgr->getRootSceneNode()->createChild());
	ParticleSystem* p = mSceneMgr->createParticleSystem(
		"explosion", "Examples/GreenyNimbus");

	ParticleEmitter *e = p->getEmitter(0);
	e->setEnabled(false);
	mParticleNode->attachObject(p);
	mParticleNode->setPosition(Vector3::ZERO);
	mParticleNode->setVisible(false);
}

void BasicTutorial_00::setOffParticleSystem(
	Ogre::SceneNode *fNode, const Ogre::String &pname, const Ogre::Vector3 &pos) 
{
	fNode->setPosition(pos);
	fNode->setVisible(true);
	MovableObject *s = fNode->getAttachedObject(pname);
	ParticleSystem* p =static_cast<ParticleSystem*>(s);
	if (p == NULL) return;
	p->setVisible(true);
	ParticleEmitter *e = p->getEmitter(0);
	e->setEnabled(true);
}

int main(int argc, char *argv[]) {
	BasicTutorial_00 app;
	app.go();  
	return 0;
}
