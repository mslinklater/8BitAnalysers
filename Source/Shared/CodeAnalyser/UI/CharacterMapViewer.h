#pragma once

#include "../../Util/GraphicsView.h"

struct FCodeAnalysisViewState;

void DrawMaskInfoComboBox(EMaskInfo* pValue);
void DrawColourInfoComboBox(EColourInfo* pValue);
void DrawCharacterSetComboBox(FCodeAnalysisState& state, uint16_t* pAddr);

void DrawCharacterMapViewer(FCodeAnalysisState& state, FCodeAnalysisViewState& viewState);
