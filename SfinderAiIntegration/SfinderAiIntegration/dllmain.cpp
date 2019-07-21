#include <cstring>
#include <vector>
#include <array>

#include "sfinder-cpp/src/sfinder/perfect_clear/full_finder.hpp"

#include "pch.h"
#include "sfinder-cpp/src/sfinder/percentage.hpp"

#define DLL_MAIN extern "C" __declspec(dllexport)

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

DLL_MAIN int add(int x, int y)
{
	return x + y;
}

// ================================================

/// スタックを食うので外出し
auto factory = core::Factory::create();
auto moveGenerator = core::srs::MoveGenerator(factory);
auto finder = sfinder::perfect_clear::Finder<core::srs::MoveGenerator>(factory, moveGenerator);

/// パフェ率を計算します。
DLL_MAIN double solution_finder_perfect()
{
	/// とりあえずテストコードそのまま
	auto permutationVector = std::vector{
		sfinder::Permutation::create<2>(std::array<core::PieceType, 2>{core::PieceType::L, core::PieceType::J}, 2),
		sfinder::Permutation::create<7>(core::kAllPieceType, 4)
	};
	auto permutations = sfinder::Permutations::create(permutationVector);

	const int maxDepth = 5;
	const int maxLine = 4;

	auto reverseLookup = sfinder::ReverseLookup::create(maxDepth, permutations.depth());

	auto percentage = sfinder::Percentage<>(finder, permutations, reverseLookup);

	/// sが使えなかったので消した。(動き的には問題ないはず)
	auto field = core::createField(""
		"XX_______X"
		"XXX______X"
		"XXXX___XXX"
		"XXX____XXX"
		""
	);
	return percentage.run(field, maxDepth, maxLine) / static_cast<double>(permutations.size());
}
