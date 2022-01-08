//
// Created by ounols on 19. 3. 31.
//

#pragma once


#include <utility>

#include "../../Matrix.h"
#include "../XML/XML.h"

namespace CSE {

	struct JointTransformData {
		std::string jointNameId;
		int jointId;
		mat4 jointLocalTransform;

		JointTransformData(int jointId, std::string jointNameId, const mat4& jointLocalTransform) :
			jointNameId(std::move(jointNameId)),
			jointId(jointId),
			jointLocalTransform(jointLocalTransform) {
		}
	};

	struct KeyFrameData {
		float time = 0;
		std::vector<JointTransformData*> jointTransforms;

		explicit KeyFrameData(float time) : time(time) {
		}
	};

	struct AnimationData {
		float lengthSeconds;
		std::vector<KeyFrameData*> keyFrames;

		AnimationData(float lengthSeconds, std::vector<KeyFrameData*> keyFrames) :
			lengthSeconds(lengthSeconds),
			keyFrames(std::move(keyFrames)) {
		}
	};

	class DAEAnimationLoader {
	public:
		DAEAnimationLoader();

		~DAEAnimationLoader();

		bool Load(const char* path, std::string name);

		AnimationData* GetAnimation() const {
			return m_animationData;
		}

	private:
		void LoadAnimation();

		std::string findRootJointName();

		std::vector<float> getKeyTimes();

		static std::vector<KeyFrameData*> initKeyFrames(const std::vector<float>& times);

		static void loadJointTransforms(std::vector<KeyFrameData*> frames, const XNode& jointData,
		                         const std::string& rootNodeId);

		static std::string getJointName(const XNode& jointData);

		static std::string getDataId(const XNode& jointData);

		static void
		processTransforms(const std::string& jointName, std::vector<float> rawData,
		                  std::vector<KeyFrameData*> keyFrames, bool root);

	private:
		std::string m_name;
		const XNode* m_root{};
		XNode m_joint;
		XNode m_animation;

		AnimationData* m_animationData = nullptr;
	};

}
