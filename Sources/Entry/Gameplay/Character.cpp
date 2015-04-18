#include "Character.hpp"
#include <Game/ScriptObject.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <scriptarray/scriptarray.h>
#include <angelscript.h>

using namespace Gameplay;

namespace
{
	void* getCharacter()
	{
		asIScriptObject* obj = reinterpret_cast<asIScriptObject*>(asGetActiveContext()->GetThisPointer());
		Kunlaboro::Component* sobj = reinterpret_cast<Kunlaboro::Component*>(obj->GetObjectType()->GetUserData((uintptr_t)obj));

		return sobj->getEntitySystem()->getAllComponentsOnEntity(sobj->getOwnerId(), "Fallacy.Character")[0];
	}

	CScriptArray* findInRadius(const Math::Vec2& pos, float radius)
	{
		std::list<Character*> list;

		asIScriptObject* obj = reinterpret_cast<asIScriptObject*>(asGetActiveContext()->GetThisPointer());
		Kunlaboro::Component* sobj = reinterpret_cast<Kunlaboro::Component*>(obj->GetObjectType()->GetUserData((uintptr_t)obj));

		sobj->sendMessage<void, std::list<Character*>&>("Fallacy.GetEnemies", list);

		auto ret = CScriptArray::Create(obj->GetEngine()->GetObjectTypeByDecl("array<Character@>"));
		for (auto& val : list)
			ret->InsertLast(val);

		return ret;
	}

#ifndef AS_SUPPORT_VALRET
	void findInRadius_generic(asIScriptGeneric* gen)
	{
		Math::Vec2* v = reinterpret_cast<Math::Vec2*>(gen->GetArgObject(0));
		float rad = gen->GetArgFloat(1);

		gen->SetReturnObject(findInRadius(*v, rad));
	}
#endif

}

void Character::addScript(asIScriptEngine* eng)
{
	eng->RegisterObjectType("Character", 0, asOBJ_REF | asOBJ_NOCOUNT);
	eng->RegisterGlobalFunction("Character@ GetCharacter()", asFUNCTION(getCharacter), asCALL_CDECL);

	eng->RegisterObjectMethod("Character", "const Vec2& get_AimVec() const", asMETHOD(Character, getAimVec), asCALL_THISCALL);
	eng->RegisterObjectMethod("Character", "const Vec2& get_Position() const", asMETHOD(Character, getPosition), asCALL_THISCALL);
	eng->RegisterObjectMethod("Character", "const Vec2& get_Velocity() const", asMETHOD(Character, getVelocity), asCALL_THISCALL);
	eng->RegisterObjectMethod("Character", "void set_AimVec(Vec2&in) const", asMETHOD(Character, setAimVec), asCALL_THISCALL);
	eng->RegisterObjectMethod("Character", "void set_Position(Vec2&in) const", asMETHOD(Character, setPosition), asCALL_THISCALL);
	eng->RegisterObjectMethod("Character", "void set_Velocity(Vec2&in) const", asMETHOD(Character, setVelocity), asCALL_THISCALL);
	eng->RegisterObjectMethod("Character", "float get_Radius() const", asMETHOD(Character, getRadius), asCALL_THISCALL);
	eng->RegisterObjectMethod("Character", "void set_Radius(float) const", asMETHOD(Character, setRadius), asCALL_THISCALL);

#ifdef AS_SUPPORT_VALRET
	eng->RegisterGlobalFunction("array<Character@>@ FindInRadius(Vec2&in, float)", asFUNCTION(findInRadius), asCALL_CDECL);
#else
	eng->RegisterGlobalFunction("array<Character@>@ FindInRadius(Vec2&in, float)", asFUNCTION(findInRadius_generic), asCALL_GENERIC);
#endif
}

Character::Character() : Kunlaboro::Component("Fallacy.Character"),
	mRadius(0), mHealth(1), mSanity(1), mMaxSanity(1)
{

}
Character::~Character()
{

}

void Character::addedToEntity()
{
	requestMessage("SetPosition", &Character::setPosition, true);
	requestMessage("GetPosition", &Character::getPositionMsg, true);
	requestMessage("SetRadius", &Character::setRadius, true);
	requestMessage("GetRadius", &Character::getRadiusMsg , true);

	requestMessage("Game.Draw", &Character::draw);
}

float Character::getRadius() const
{
	return mRadius;
}

void Character::setRadius(float r)
{
	mRadius = r;
}

const Math::Vec2& Character::getAimVec() const
{
	return mAimVec;
}
const Math::Vec2& Character::getPosition() const
{
	return mPosition;
}
const Math::Vec2& Character::getVelocity() const
{
	return mVelocity;
}
void Character::setAimVec(const Math::Vec2& aim)
{
	if (aim != Math::Vec2() && mAimVec == Math::Vec2())
	{
		// Start firing
	}
	else if (aim == Math::Vec2() && mAimVec != Math::Vec2())
	{
		// Stop firing
	}

	mAimVec = aim;
}
void Character::setPosition(const Math::Vec2& pos)
{
	mPosition = pos;
}
void Character::setVelocity(const Math::Vec2& v)
{
	mVelocity = v;
}

void Character::draw(sf::RenderTarget& target)
{
	sf::CircleShape shape(16);
	shape.setOrigin(16, 16);
	shape.setPosition(mPosition);

	target.draw(shape);
}

Kunlaboro::Optional<Math::Vec2> Character::getPositionMsg()
{
	return mPosition;
}

Kunlaboro::Optional<float> Character::getRadiusMsg()
{
	return mRadius;
}
