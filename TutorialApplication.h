//
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
// Date: 2018/10/08
//
// Select and move characters
#ifndef __BasicTutorial_00_h_
#define __BasicTutorial_00_h_
 
#include "BaseApplication.h"
#include "selection_rectangle.h"
#include "sound.h"

using namespace Ogre;
#define MOTION_TYPE_NONE 0
#define MOTION_TYPE_IDLE 1
#define MOTION_TYPE_WALK 2

class BasicTutorial_00 : public BaseApplication
{
private:
	void initParticleSystemForExplosion();
	void setOffParticleSystem(
	Ogre::SceneNode *fNode, const Ogre::String &pname, const Ogre::Vector3 &pos); 

	SOUND *mSound;
	SceneNode *mParticleNode;

	SelectionRectangle *mSelectRect;
	Ogre::PlaneBoundedVolumeListSceneQuery *mVolQuery;
	Ogre::RaySceneQuery *mRayQuery;
	Ogre::SceneQueryResultMovableList mSelectItem;
	float lightAngle, lightRotateSpeed, lightRadius;

	int MOVABLE_MASK;
	int PLANE_MASK;
	bool isPress, isMoving;
	Ogre::Vector2 startPoint, endPoint;

	void createCamera00();
	void createCamera01();
	virtual void createCamera();
	void createViewport00();
	void createViewport01();
	virtual void createViewports();
	virtual bool mouseMoved( const OIS::MouseEvent &arg );
    virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
    virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	virtual bool frameStarted(const FrameEvent &evt);

	void stringCreate(const Ogre::String & prefix, int index, Ogre::String &out_name);
	void singleClickSelect();
	void volumeSelect();
	void selectItem(SceneQueryResult &result);
	void deselectItem();

public:
	BasicTutorial_00(void);
	virtual void createScene(void);
};
 
#endif // #ifndef __BasicTutorial_00_h_