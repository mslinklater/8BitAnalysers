#include "../../CodeAnalyser.h"
#include "../CodeAnalyserUI.h"

#include <Util/Misc.h>
#include <chips/z80.h>
#include <imgui.h>

// took these out of the chips debugger
uint16_t InputU16(const char* label, uint16_t val) 
{
	char buf[5];
	for (int i = 0; i < 4; i++) {
		buf[i] = "0123456789ABCDEF"[val >> ((3 - i) * 4) & 0xF];
	}
	buf[4] = 0;
	const int flags = ImGuiInputTextFlags_CharsHexadecimal |
		ImGuiInputTextFlags_CharsUppercase |
		ImGuiInputTextFlags_EnterReturnsTrue;
	ImGui::PushItemWidth(38);
	if (ImGui::InputText(label, buf, sizeof(buf), flags)) {
		int res;
		if (sscanf_s(buf, "%X", &res) == 1) {
			val = (uint16_t)res;
		}
	}
	ImGui::PopItemWidth();
	return val;
}

uint8_t InputU8(const char* label, uint8_t val)
{
	char buf[3];
	for (int i = 0; i < 2; i++) {
		buf[i] = "0123456789ABCDEF"[val >> ((1 - i) * 4) & 0xF];
	}
	buf[2] = 0;
	const int flags = ImGuiInputTextFlags_CharsHexadecimal |
		ImGuiInputTextFlags_CharsUppercase |
		ImGuiInputTextFlags_EnterReturnsTrue;
	ImGui::PushItemWidth(22);
	if (ImGui::InputText(label, buf, sizeof(buf), flags)) {
		int res;
		if (sscanf_s(buf, "%X", &res) == 1) {
			val = (uint8_t)res;
		}
	}
	ImGui::PopItemWidth();
	return val;
}

// structure to hold registers for display purposes
struct FZ80DisplayRegisters
{
	FZ80DisplayRegisters() {}
	FZ80DisplayRegisters(z80_t* pCPU)
	{
		A = z80_a(pCPU);
		B = z80_b(pCPU);
		C = z80_c(pCPU);
		D = z80_d(pCPU);
		E = z80_e(pCPU);
		H = z80_h(pCPU);
		L = z80_l(pCPU);
		BC = z80_bc(pCPU);
		DE = z80_de(pCPU);
		HL = z80_hl(pCPU);
		IX = z80_ix(pCPU);
		IY = z80_iy(pCPU);
		SP = z80_sp(pCPU);
		PC = z80_pc(pCPU);
		I = z80_i(pCPU);
		R = z80_r(pCPU);
		IM = z80_im(pCPU);

		// Flags
		const uint8_t f = z80_f(pCPU);
		CarryFlag = !!(f & Z80_CF);
		AddSubtractFlag = !!(f & Z80_NF);
		ParityOverflowFlag = !!(f & Z80_VF);
		HalfCarryFlag = !!(f & Z80_HF);
		ZeroFlag = !!(f & Z80_ZF);
		SignFlag = !!(f & Z80_SF);

		IFF1 = z80_iff1(pCPU);
		IFF2 = z80_iff2(pCPU);
	}

	uint8_t	A = 0, B = 0, C = 0, D = 0, E = 0, H = 0, L = 0;
	uint8_t I = 0, R = 0, IM = 0;
	uint16_t BC = 0, DE = 0, HL= 0, IX = 0, IY = 0;
	uint16_t SP = 0, PC = 0;

	// Flags
	bool	CarryFlag = false;
	bool	AddSubtractFlag = false;
	bool	ParityOverflowFlag = false;
	bool	HalfCarryFlag = false;
	bool	ZeroFlag = false;
	bool	SignFlag = false;

	bool	IFF1 = false;
	bool	IFF2 = false;
};

FZ80DisplayRegisters g_OldRegs;

void StoreRegisters_Z80(FCodeAnalysisState& state)
{
	g_OldRegs = FZ80DisplayRegisters((z80_t*)state.CPUInterface->GetCPUEmulator());
}

void DrawRegisters_Z80(FCodeAnalysisState& state)
{
	FCodeAnalysisViewState& viewState = state.GetFocussedViewState();
	z80_t* pCPU = (z80_t*)state.CPUInterface->GetCPUEmulator();
	const ImVec4 regNormalCol(1.0f, 1.0f, 1.0f, 1.0f);
	const ImVec4 regChangedCol(1.0f, 1.0f, 0.0f, 1.0f);
	ImVec4 regColour = regNormalCol;

	FZ80DisplayRegisters curRegs(pCPU);
	const FZ80DisplayRegisters& oldRegs = g_OldRegs;

	// A
	ImGui::TextColored(curRegs.A != oldRegs.A ? regChangedCol : regNormalCol, "A:%s", NumStr(curRegs.A));

	ImGui::Separator();

	// CPU flags
	static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
	if (ImGui::BeginTable("z80flags", 6, flags))
	{
		ImGui::TableSetupColumn("Carry");
		ImGui::TableSetupColumn("Add/Sub");
		ImGui::TableSetupColumn("Par/Ovr");
		ImGui::TableSetupColumn("H-Carry");
		ImGui::TableSetupColumn("Zero");
		ImGui::TableSetupColumn("Sign");
		ImGui::TableHeadersRow();

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::TextColored(curRegs.CarryFlag != oldRegs.CarryFlag ? regChangedCol : regNormalCol, "%s", curRegs.CarryFlag ? "Y" : "N");
		ImGui::TableSetColumnIndex(1);
		ImGui::TextColored(curRegs.AddSubtractFlag != oldRegs.AddSubtractFlag ? regChangedCol : regNormalCol, "%s", curRegs.AddSubtractFlag ? "Y" : "N");
		ImGui::TableSetColumnIndex(2);
		ImGui::TextColored(curRegs.ParityOverflowFlag != oldRegs.ParityOverflowFlag ? regChangedCol : regNormalCol, "%s", curRegs.ParityOverflowFlag ? "Y" : "N");
		ImGui::TableSetColumnIndex(3);
		ImGui::TextColored(curRegs.HalfCarryFlag != oldRegs.HalfCarryFlag ? regChangedCol : regNormalCol, "%s", curRegs.HalfCarryFlag ? "Y" : "N");
		ImGui::TableSetColumnIndex(4);
		ImGui::TextColored(curRegs.ZeroFlag != oldRegs.ZeroFlag ? regChangedCol : regNormalCol, "%s", curRegs.ZeroFlag ? "Y" : "N");
		ImGui::TableSetColumnIndex(5);
		ImGui::TextColored(curRegs.SignFlag != oldRegs.SignFlag ? regChangedCol : regNormalCol, "%s", curRegs.SignFlag ? "Y" : "N");

		ImGui::EndTable();
	}

	ImGui::Separator();

	// B & C
	ImGui::TextColored(curRegs.B != oldRegs.B ? regChangedCol : regNormalCol, "B:%s", NumStr(curRegs.B));
	ImGui::SameLine();
	ImGui::TextColored(curRegs.C != oldRegs.C ? regChangedCol : regNormalCol, "C:%s", NumStr(curRegs.C));
	ImGui::SameLine();
	ImGui::TextColored(curRegs.BC != oldRegs.BC ? regChangedCol : regNormalCol, "BC:%s", NumStr(curRegs.BC));
	DrawAddressLabel(state, viewState, curRegs.BC);

	ImGui::Separator();

	// D & E
	ImGui::TextColored(curRegs.D != oldRegs.D ? regChangedCol : regNormalCol, "D:%s", NumStr(curRegs.D));
	ImGui::SameLine();
	ImGui::TextColored(curRegs.E != oldRegs.E ? regChangedCol : regNormalCol, "E:%s", NumStr(curRegs.E));
	ImGui::SameLine();
	ImGui::TextColored(curRegs.DE != oldRegs.DE ? regChangedCol : regNormalCol, "DE:%s", NumStr(curRegs.DE));
	DrawAddressLabel(state, viewState, curRegs.DE);
	
	ImGui::Separator();

	// H & L
	ImGui::TextColored(curRegs.H != oldRegs.H ? regChangedCol : regNormalCol, "H:%s", NumStr(curRegs.H));
	ImGui::SameLine();
	ImGui::TextColored(curRegs.L != oldRegs.L ? regChangedCol : regNormalCol, "L:%s", NumStr(curRegs.L));
	ImGui::SameLine();
	ImGui::TextColored(curRegs.HL != oldRegs.HL ? regChangedCol : regNormalCol, "HL:%s", NumStr(curRegs.HL));
	DrawAddressLabel(state, viewState, curRegs.HL);

	ImGui::Separator();
	
	// IX & IY
	
	// IX
	ImGui::TextColored(curRegs.IX != oldRegs.IX ? regChangedCol : regNormalCol, "IX:%s", NumStr(curRegs.IX));
	DrawAddressLabel(state, viewState, curRegs.IX);
	ImGui::SameLine();
	// IY
	ImGui::TextColored(curRegs.IY != oldRegs.IY ? regChangedCol : regNormalCol, "IY:%s", NumStr(curRegs.IY));
	DrawAddressLabel(state, viewState, curRegs.IY);

	ImGui::Separator();
	
	// SP & PC
	ImGui::TextColored(curRegs.SP != oldRegs.SP ? regChangedCol : regNormalCol, "SP:%s", NumStr(curRegs.SP));
	DrawAddressLabel(state, viewState, curRegs.SP);
	ImGui::SameLine();
	ImGui::TextColored(curRegs.PC != oldRegs.PC ? regChangedCol : regNormalCol, "PC:%s", NumStr(curRegs.PC));
	DrawAddressLabel(state, viewState, curRegs.PC);

	ImGui::Separator();

	// R
	ImGui::TextColored(curRegs.R != oldRegs.R ? regChangedCol : regNormalCol, "R:%s", NumStr(curRegs.R));

	ImGui::Separator();

	// Interrupt related
	// I
	ImGui::TextColored(curRegs.I != oldRegs.I ? regChangedCol : regNormalCol, "I:%s", NumStr(curRegs.I));
	ImGui::SameLine();
	// IM
	ImGui::TextColored(curRegs.IM != oldRegs.IM ? regChangedCol : regNormalCol, "IM:%s", NumStr(curRegs.IM));
	ImGui::SameLine();
	// IFF 1 & 2
	ImGui::TextColored(curRegs.IFF1 != oldRegs.IFF1 ? regChangedCol : regNormalCol, "IFF1:%s", curRegs.IFF1 ? "Y" : "N");
	ImGui::SameLine();
	ImGui::TextColored(curRegs.IFF2 != oldRegs.IFF2 ? regChangedCol : regNormalCol, "IFF2:%s", curRegs.IFF2 ? "Y" : "N");

	
}