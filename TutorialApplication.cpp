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
	isMouseMoving = false;
	isRobotMoving = false;

	MOVABLE_MASK = 1 << 10;
	PLANE_MASK = 1 << 11;

	//mSelectItem = SceneQueryResultMovableList();
}

void BasicTutorial_00::createCamera00()
{
	mCamera = mSceneMgr->createCamera("PlayerCamera");

	mCamera->setPosition(Ogre::Vector3(0,700,800));
	mCamera->lookAt(Ogre::Vector3(0,0,0));

	mCamera->setNearClipDistance(5);

	mCameraMan = new OgreBites::SdkCameraMan(mCamera);   // create a default camera controller
}

void BasicTutorial_00::createCamera01()
{
	mCamera = mSceneMgr->createCamera("UpperCamera");

	mCamera->setPosition(Ogre::Vector3(0,1400,1));
	mCamera->lookAt(Ogre::Vector3(0,0,0));

	mCamera->setNearClipDistance(5);
	
}

void BasicTutorial_00::createCamera(void)
{
	createCamera01();
	createCamera00();
}

void BasicTutorial_00::createViewport00()
{
	Ogre::Viewport* vp;
	// Create one viewport, entire window
	
	vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio(
		Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}

void BasicTutorial_00::createViewport01()
{
	Ogre::Viewport* vp;
	// Create one viewport, entire window
	Camera *cam = mSceneMgr->getCamera("UpperCamera");
	vp = mWindow->addViewport(cam, 1, 0.75, 0, 0.25, 0.25);
	vp->setBackgroundColour(Ogre::ColourValue(1, 1, 0));
	vp->setOverlaysEnabled(false);
	vp->setSkiesEnabled(false);

	//cam->lookAt(0, 0, 0);

	// Alter the camera aspect ratio to match the viewport
	cam->setAspectRatio(
		4 * Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
	
	std::cout << "Cameera : " << cam->getDirection() << std::endl;
}

void BasicTutorial_00::createViewports(void)
{
	createViewport00();
	createViewport01();
}

void BasicTutorial_00::createScene(void) 
{
	mSceneMgr->setAmbientLight( ColourValue( 0.9, 0.9, 0.9 ) ); 
	//mSceneMgr->setFog(Ogre::FOG_LINEAR, Ogre::ColourValue(1, 1, 1), 0, 1400, 1600);
	mSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox");
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

	// Selection Box
	mSelectRect = new SelectionRectangle("SelectionRectangle");
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(mSelectRect);

	mVolQuery = mSceneMgr->createPlaneBoundedVolumeQuery(Ogre::PlaneBoundedVolumeList());
	mVolQuery->setQueryMask(MOVABLE_MASK);

	mRayQuery = mSceneMgr->createRayQuery(Ray());
	mRayQuery->setQueryMask(MOVABLE_MASK);

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
	ent->setQueryFlags(PLANE_MASK);
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
		ent->setQueryFlags(MOVABLE_MASK);

		SceneNode *snode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		snode->attachObject(ent);
		snode->setPosition(radius * cos(radian * step * i), 0, radius * sin(radian * step * i));
		snode->lookAt(Ogre::Vector3(0, 0, 0), Ogre::Node::TransformSpace::TS_WORLD, Ogre::Vector3::UNIT_Z);
		snode->rotate(Ogre::Quaternion(sqrt(0.5), 0, -sqrt(0.5), 0));

		// Add entity into vector
		robots.push_back(ent);
	}

	radius = 300;
	for (int i=0; i<robot_num; i++) {
		Ogre::String ent_name;
		stringCreate("outer_robot_", i, ent_name);
		std::cout << cos(radian * step * i) << " " << sin(radian * step * i) << std::endl;

		ent = mSceneMgr->createEntity(ent_name, "robot.mesh");
		ent->setQueryFlags(MOVABLE_MASK);
		SceneNode *snode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		snode->attachObject(ent);
		snode->setPosition(radius * cos(radian * step * i), 0, radius * sin(radian * step * i));
		snode->lookAt(Ogre::Vector3(0, 0, 0), Ogre::Node::TransformSpace::TS_WORLD, Ogre::Vector3::UNIT_Z);
		snode->rotate(Ogre::Quaternion(sqrt(0.5), 0, -sqrt(0.5), 0));
		if (!i) {
			snode->scale(2, 2, 2);
		}

		// Add entity into vector
		robots.push_back(ent);
	}

	// Sphere
	ent = mSceneMgr->createEntity("center_sphere", "sphere.mesh");
	ent->setQueryFlags(0);
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
		isMouseMoving = true;
		Ogre::Ray mRay = mTrayMgr->getCursorRay(mCamera);
		endPoint = mTrayMgr->sceneToScreen(mCamera, mRay.getOrigin());

		mSelectRect->setVisible(true);
		mSelectRect->setCorners(min(startPoint.x, endPoint.x), min(startPoint.y, endPoint.y), max(startPoint.x, endPoint.x), max(startPoint.y, endPoint.y));
	}
	return BaseApplication::mouseMoved( arg);
}

void BasicTutorial_00::singleClickSelect()
{
	mRayQuery->setQueryMask(MOVABLE_MASK);
	Ray ray = mCamera->getCameraToViewportRay(startPoint.x, 1.0+startPoint.y);

	mRayQuery->setRay(ray);
	mRayQuery->setSortByDistance(true);

	RaySceneQueryResult &result = mRayQuery->execute();
	
	deselectItem();
	if (result.begin() != result.end() && result.begin()->movable) {
		mSelectItem.push_back(result.begin()->movable);
		result.begin()->movable->getParentSceneNode()->showBoundingBox(true);
	}
}

void BasicTutorial_00::volumeSelect()
{
	float left = min(startPoint.x, endPoint.x), 
		top = 1.0+min(startPoint.y, endPoint.y), 
		right = max(startPoint.x, endPoint.x), 
		bottom = 1.0+max(startPoint.y, endPoint.y);

	//std::cout << left << " " << top << " " << right << " " << bottom << std::endl;
	Ray topLeft = mCamera->getCameraToViewportRay(left, top);
	Ray topRight = mCamera->getCameraToViewportRay(right, top);
	Ray bottomLeft = mCamera->getCameraToViewportRay(left, bottom);
	Ray bottomRight = mCamera->getCameraToViewportRay(right, bottom);
	// The plane faces the counter clockwise position.
	PlaneBoundedVolume vol;
	int np = 5000;
	vol.planes.push_back(Plane(topLeft.getPoint(3), topRight.getPoint(3), bottomRight.getPoint(3)));         // front plane
	vol.planes.push_back(Plane(topLeft.getOrigin(), topLeft.getPoint(np), topRight.getPoint(np)));         // top plane
	vol.planes.push_back(Plane(topLeft.getOrigin(), bottomLeft.getPoint(np), topLeft.getPoint(np)));       // left plane
	vol.planes.push_back(Plane(bottomLeft.getOrigin(), bottomRight.getPoint(np), bottomLeft.getPoint(np)));   // bottom plane
	vol.planes.push_back(Plane(bottomRight.getOrigin(), topRight.getPoint(np), bottomRight.getPoint(np)));     // right plane 

	PlaneBoundedVolumeList volList;
	volList.push_back(vol);
	mVolQuery->setVolumes(volList);
	SceneQueryResult &result = mVolQuery->execute();

	deselectItem();
	selectItem(result);
}

bool BasicTutorial_00::raycastPlane(Ogre::Vector3 &hitPoint)
{
	mRayQuery->setQueryMask(PLANE_MASK);
	Ray ray = mCamera->getCameraToViewportRay(startPoint.x, 1.0+startPoint.y);
	mRayQuery->setRay(ray);
	mRayQuery->setSortByDistance(true);

	RaySceneQueryResult &result = mRayQuery->execute();
	if (result.begin() != result.end() && result.begin()->movable) {
		hitPoint = ray.getPoint(result.begin()->distance);
		hitPoint.y = 0;
		return true;
	}

	return false;
}

void BasicTutorial_00::selectItem(SceneQueryResult &result)
{
	SceneQueryResultMovableList::iterator it = result.movables.begin();
	for (;it != result.movables.end(); it++) {
		(*it)->getParentSceneNode()->showBoundingBox(true);
		mSelectItem.push_back((*it));
	}
}

void BasicTutorial_00::deselectItem()
{
	SceneQueryResultMovableList::iterator it = mSelectItem.begin();
	for (;it != mSelectItem.end(); it++) {
		(*it)->getParentSceneNode()->showBoundingBox(false);
	}
	mSelectItem.clear();
}

bool BasicTutorial_00::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if (isRobotMoving) {
		
	}
	else if (!isMouseMoving || (fabs(startPoint.x - endPoint.x) < 0.005 && fabs(endPoint.y - startPoint.y) < 0.005)) {
		std::cout << "Is Single Press\n";
		singleClickSelect();
	}
	else {
		volumeSelect();
	}

	isMouseMoving = isPress = false;
	mSelectRect->setVisible(false);
	return BaseApplication::mouseReleased( arg, id );
}

bool BasicTutorial_00::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	Ogre::Ray mRay = mTrayMgr->getCursorRay(mCamera);
	endPoint = startPoint = mTrayMgr->sceneToScreen(mCamera, mRay.getOrigin());

	if (id == OIS::MB_Right) {
		isRobotMoving = true;
		return BaseApplication::mousePressed( arg, id );
	}

	isPress = true;
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

	// Robot Animation
	for (int i=0;i<robots.size();i++) {
		if (robots[i]->getAnimationState("Walk")->getEnabled()) continue;
		Ogre::AnimationState *animationState = robots[i]->getAnimationState("Idle");
		animationState->setLoop(true);
		animationState->setEnabled(true);
		animationState->addTime(evt.timeSinceLastFrame);
	}
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
