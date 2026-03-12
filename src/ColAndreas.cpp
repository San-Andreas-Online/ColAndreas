//*********************************************************************************************************//
// ColAndreas by [uL]Pottus & [uL]Chris42O & [uL]Slice ****************************************************//
//*********************************************************************************************************//

#include <cstdarg>
#include <sdk.hpp>
#include <Server/Components/Pawn/pawn.hpp>
#include <btBulletDynamicsCommon.h>
#include "ColAndreas.h"
#include "DynamicWorld.h"
#include "Natives.h"

bool colInit = false;
bool colDataLoaded = false;
cell nullAddress = 0;

ColAndreasWorld* collisionWorld = nullptr;

logprintf_t logprintf = nullptr;

extern void *pAMXFunctions;

namespace
{
	ICore *gCore = nullptr;

	void ComponentLogPrintf(char *format, ...)
	{
		if (gCore == nullptr)
		{
			return;
		}

		va_list args;
		va_start(args, format);
		gCore->vprintLn(format, args);
		va_end(args);
	}

	const AMX_NATIVE_INFO PluginNatives[] = {
		// Standard Natives
		{ "CA_Init", ColAndreasNatives::CA_Init },
		{ "CA_RayCastLine", ColAndreasNatives::CA_RayCastLine },
		{ "CA_RayCastLineExtraID", ColAndreasNatives::CA_RayCastLineExtraID },
		{ "CA_RayCastLineID", ColAndreasNatives::CA_RayCastLineID },
		{ "CA_RayCastLineAngle", ColAndreasNatives::CA_RayCastLineAngle },
		{ "CA_RayCastMultiLine", ColAndreasNatives::CA_RayCastMultiLine },
		{ "CA_RayCastReflectionVector", ColAndreasNatives::CA_RayCastReflectionVector },
		{ "CA_RayCastLineNormal", ColAndreasNatives::CA_RayCastLineNormal },
		{ "CA_ContactTest", ColAndreasNatives::CA_ContactTest },
		{ "CA_LoadFromDff", ColAndreasNatives::CA_LoadFromDff },
		{ "CA_CreateObject", ColAndreasNatives::CA_CreateObject },
		{ "CA_DestroyObject", ColAndreasNatives::CA_DestroyObject },
		{ "CA_IsValidObject", ColAndreasNatives::CA_IsValidObject },
		{ "CA_EulerToQuat", ColAndreasNatives::CA_EulerToQuat },
		{ "CA_QuatToEuler", ColAndreasNatives::CA_QuatToEuler },
		{ "CA_RemoveBuilding", ColAndreasNatives::CA_RemoveBuilding },
		{ "CA_RestoreBuilding", ColAndreasNatives::CA_RestoreBuilding },
		{ "CA_SetObjectPos", ColAndreasNatives::CA_SetObjectPos },
		{ "CA_SetObjectRot", ColAndreasNatives::CA_SetObjectRot },
		{ "CA_GetModelBoundingSphere", ColAndreasNatives::CA_GetModelBoundingSphere },
		{ "CA_GetModelBoundingBox", ColAndreasNatives::CA_GetModelBoundingBox },
		{ "CA_SetObjectExtraID", ColAndreasNatives::CA_SetObjectExtraID },
		{ "CA_GetObjectExtraID", ColAndreasNatives::CA_GetObjectExtraID },

		// Extended Natives
		{ "CA_RayCastLineEx", ColAndreasNatives::CA_RayCastLineEx },
		{ "CA_RayCastLineAngleEx", ColAndreasNatives::CA_RayCastLineAngleEx },
		{ nullptr, nullptr }
	};

	class ColAndreasComponent final : public IComponent, public CoreEventHandler, public PawnEventHandler
	{
		PROVIDE_UID(0xEC653C095187A61F);

	public:
		StringView componentName() const override
		{
			return "open.mp ColAndreas";
		}

		SemanticVersion componentVersion() const override
		{
			return SemanticVersion(1, 5, 0, 0);
		}

		void onLoad(ICore *core) override
		{
			core_ = core;
			gCore = core_;
			logprintf = ComponentLogPrintf;
			pAMXFunctions = nullptr;
			nullAddress = 0;
			colInit = false;
			colDataLoaded = false;

			core_->printLn("*********************");
			core_->printLn("** Created By:     **");
			core_->printLn("** [uL]Chris42O    **");
			core_->printLn("** [uL]Slice       **");
			core_->printLn("** [uL]Pottus      **");
			core_->printLn("*********************");

			collisionWorld = new ColAndreasWorld();
			if (collisionWorld->loadCollisionData())
			{
				core_->printLn("Loaded collision data.");
				colDataLoaded = true;
			}
			else
			{
				core_->printLn("No collision data found.");
			}

			core_->printLn("*********************");
			core_->printLn("  ColAndreas Loaded");
			core_->printLn("   " CA_VERSION);
			core_->printLn("*********************");
		}

		void onInit(IComponentList *components) override
		{
			pawnComponent_ = components->queryComponent<IPawnComponent>();
			if (pawnComponent_ == nullptr)
			{
				core_->logLn(LogLevel::Error, "component.colandreas: Pawn component not loaded.");
				return;
			}

			pAMXFunctions = static_cast<void *>(
				const_cast<void **>(pawnComponent_->getAmxFunctions().data()));

			pawnComponent_->getEventDispatcher().addEventHandler(this);
			core_->getEventDispatcher().addEventHandler(this);
			eventsRegistered_ = true;

			if (IPawnScript *script = pawnComponent_->mainScript())
			{
				RegisterScript(*script);
			}

			for (IPawnScript *script : pawnComponent_->sideScripts())
			{
				if (script != nullptr)
				{
					RegisterScript(*script);
				}
			}
		}

		void onTick(Microseconds elapsed, TimePoint now) override
		{
			(void)elapsed;
			(void)now;
		}

		void onAmxLoad(IPawnScript &script) override
		{
			RegisterScript(script);
		}

		void onAmxUnload(IPawnScript &script) override
		{
			(void)script;
		}

		void onFree(IComponent *component) override
		{
			if (component == pawnComponent_)
			{
				if (pawnComponent_ != nullptr && eventsRegistered_)
				{
					pawnComponent_->getEventDispatcher().removeEventHandler(this);
				}
				pawnComponent_ = nullptr;
				pAMXFunctions = nullptr;
			}
		}

		void reset() override
		{
		}

		void free() override
		{
			Shutdown();
			delete this;
		}

		~ColAndreasComponent() override = default;

	private:
		void RegisterScript(IPawnScript &script)
		{
			AMX *amx = script.GetAMX();
			if (amx == nullptr)
			{
				return;
			}

			const int error = amx_Register(amx, PluginNatives, -1);
			if (error != AMX_ERR_NONE && error != AMX_ERR_NOTFOUND)
			{
				core_->logLn(LogLevel::Error,
					"component.colandreas: amx_Register failed (error %d).",
					error);
			}
		}

		void Shutdown()
		{
			if (eventsRegistered_)
			{
				if (pawnComponent_ != nullptr)
				{
					pawnComponent_->getEventDispatcher().removeEventHandler(this);
				}
				if (core_ != nullptr)
				{
					core_->getEventDispatcher().removeEventHandler(this);
				}
				eventsRegistered_ = false;
			}

			if (core_ != nullptr)
			{
				core_->printLn("*********************");
				core_->printLn("*ColAndreas Unloaded*");
				core_->printLn("*********************");
			}

			if (collisionWorld != nullptr)
			{
				delete collisionWorld;
				collisionWorld = nullptr;
			}

			colInit = false;
			colDataLoaded = false;
			nullAddress = 0;
			pAMXFunctions = nullptr;
			logprintf = nullptr;
			pawnComponent_ = nullptr;
			gCore = nullptr;
		}

		ICore *core_ = nullptr;
		IPawnComponent *pawnComponent_ = nullptr;
		bool eventsRegistered_ = false;
	};
}

COMPONENT_ENTRY_POINT()
{
	return new ColAndreasComponent();
}
