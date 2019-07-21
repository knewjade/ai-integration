#include <cstring>
#include <vector>
#include <array>
#include <cassert>

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

core::PieceType charToPiece(char ch) {
	switch (ch) {
	case 'S':
		return core::PieceType::S;
	case 'Z':
		return core::PieceType::Z;
	case 'J':
		return core::PieceType::J;
	case 'L':
		return core::PieceType::L;
	case 'T':
		return core::PieceType::T;
	case 'O':
		return core::PieceType::O;
	case 'I':
		return core::PieceType::I;
	}
	assert(false);
	return static_cast<core::PieceType>(-1);
}

/// パフェ成功確率を計算します。
/// 入力できるライン数は最大で6ラインまで。
/// 入力値に問題がある場合は` 0.0より小さい値` を返します。
DLL_MAIN double calc_pc_success_rate(uint64_t low_board, const char* reminder, int max_line)
{
	// ライン数のチェック
	if (max_line <= 0 || 6 < max_line)
	{
		return -1.0;
	}

	// フィールドの定義
	const auto field = core::Field(low_board);

	// 残りの空間が4の倍数ではないとき、0.0を返却
	const auto num_blocks = field.numOfAllBlocks();
	const auto num_spaces = max_line * 10 - num_blocks;
	if (num_spaces % 4 != 0)
	{
		return 0.0;
	}

	// おくべき残りのミノの個数
	const auto max_depth = num_spaces / 4;

	// reminderの型を変換
	auto pieces = std::vector<core::PieceType>{};
	for (const char* p = &reminder[0]; *p != '\0'; ++p)
	{
		pieces.emplace_back(charToPiece(*p));
	}

	// reminderに6ミノ以上ある場合はエラー
	const int pieces_size = pieces.size();
	if (6 < pieces_size)
	{
		assert(false);
		return -1.0;
	}

	// ミノの定義
	// 全体で6ミノを置く想定。そのためreminderが2文字のとき、後ろに*p4を加える
	//  ex) [LJ]!, *p4
	auto permutation_vector = std::vector{
		sfinder::Permutation::create(pieces, pieces_size),
		sfinder::Permutation::create<7>(core::kAllPieceType, 6 - pieces_size)
	};
	const auto permutations = sfinder::Permutations::create(permutation_vector);

	// パフェ成功確率の計算
	const auto reverseLookup = sfinder::ReverseLookup::create(max_depth, permutations.depth());
	auto percentage = sfinder::Percentage<>(finder, permutations, reverseLookup);
	return percentage.run(field, max_depth, max_line) / static_cast<double>(permutations.size());
}
