#include <TApplication.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TGLabel.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TRandom.h>
#include <TGLayout.h>
#include <TGNumberEntry.h>
#include <iostream>
#include <sstream>

class HistogramViewer {
private:
    TGMainFrame* fMain;
    TRootEmbeddedCanvas* fCanvases[4];
    TH1F* fHistograms[4];
    
    // 输入控件
    TGNumberEntry* fGaussMeanEntry;
    TGNumberEntry* fGaussSigmaEntry;
    TGNumberEntry* fExpLambdaEntry;
    TGNumberEntry* fPoissonMeanEntry;
    TGNumberEntry* fUniformMinEntry;
    TGNumberEntry* fUniformMaxEntry;
    
    TGTextButton* fExitButton;

public:
    HistogramViewer(const TGWindow* parent = 0, UInt_t width = 1200, UInt_t height = 800) {
        fMain = new TGMainFrame(parent, width, height);
        fMain->SetWindowName("Real-time Histogram Viewer");
        setupGUI();
        setupHistograms();
        updateAllHistograms();
    }
    
    virtual ~HistogramViewer() {
        for (int i = 0; i < 4; i++) {
            if (fHistograms[i]) delete fHistograms[i];
        }
        if (fMain) {
            fMain->Cleanup();
            delete fMain;
        }
    }
    
    void setupGUI() {
        // 主垂直框架
        TGVerticalFrame* mainFrame = new TGVerticalFrame(fMain);
        
        // 标题
        TGLabel* title = new TGLabel(mainFrame, "Real-time Histogram Viewer with Parameter Controls");
        mainFrame->AddFrame(title, new TGLayoutHints(kLHintsCenterX | kLHintsTop, 5, 5, 10, 5));
        
        // 水平框架：画布 + 控制面板
        TGHorizontalFrame* hFrame = new TGHorizontalFrame(mainFrame);
        
        // 画布区域
        TGVerticalFrame* canvasFrame = new TGVerticalFrame(hFrame);
        
        // 2x2 画布网格
        TGHorizontalFrame* row1 = new TGHorizontalFrame(canvasFrame);
        TGHorizontalFrame* row2 = new TGHorizontalFrame(canvasFrame);
        
        fCanvases[0] = new TRootEmbeddedCanvas("Canvas1", row1, 350, 250);
        fCanvases[1] = new TRootEmbeddedCanvas("Canvas2", row1, 350, 250);
        fCanvases[2] = new TRootEmbeddedCanvas("Canvas3", row2, 350, 250);
        fCanvases[3] = new TRootEmbeddedCanvas("Canvas4", row2, 350, 250);
        
        row1->AddFrame(fCanvases[0], new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));
        row1->AddFrame(fCanvases[1], new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));
        row2->AddFrame(fCanvases[2], new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));
        row2->AddFrame(fCanvases[3], new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));
        
        canvasFrame->AddFrame(row1, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
        canvasFrame->AddFrame(row2, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
        
        // 控制面板
        TGVerticalFrame* controlPanel = new TGVerticalFrame(hFrame, 300, 600);
        
        // 标题
        TGLabel* controlTitle = new TGLabel(controlPanel, "Parameter Controls (Real-time)");
        controlPanel->AddFrame(controlTitle, new TGLayoutHints(kLHintsCenterX | kLHintsTop, 5, 5, 10, 10));
        
        // 说明标签
        TGLabel* infoLabel = new TGLabel(controlPanel, "Changes update automatically");
        controlPanel->AddFrame(infoLabel, new TGLayoutHints(kLHintsCenterX | kLHintsTop, 5, 5, 5, 10));
        
        // 高斯分布参数
        TGGroupFrame* gaussGroup = new TGGroupFrame(controlPanel, "Gaussian Distribution");
        TGHorizontalFrame* gaussFrame1 = new TGHorizontalFrame(gaussGroup);
        TGHorizontalFrame* gaussFrame2 = new TGHorizontalFrame(gaussGroup);
        
        TGLabel* gaussMeanLabel = new TGLabel(gaussFrame1, "Mean (μ):");
        fGaussMeanEntry = new TGNumberEntry(gaussFrame1, 0.0, 5, -1, TGNumberFormat::kNESReal, TGNumberFormat::kNEANonNegative);
        fGaussMeanEntry->SetLimits(TGNumberFormat::kNELLimitMinMax, -10.0, 10.0);
        
        TGLabel* gaussSigmaLabel = new TGLabel(gaussFrame2, "Sigma (σ):");
        fGaussSigmaEntry = new TGNumberEntry(gaussFrame2, 1.0, 5, -1, TGNumberFormat::kNESReal, TGNumberFormat::kNEANonNegative);
        fGaussSigmaEntry->SetLimits(TGNumberFormat::kNELLimitMinMax, 0.1, 5.0);
        
        gaussFrame1->AddFrame(gaussMeanLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 5, 2, 2));
        gaussFrame1->AddFrame(fGaussMeanEntry, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 5, 5, 2, 2));
        gaussFrame2->AddFrame(gaussSigmaLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 5, 2, 2));
        gaussFrame2->AddFrame(fGaussSigmaEntry, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 5, 5, 2, 2));
        
        gaussGroup->AddFrame(gaussFrame1, new TGLayoutHints(kLHintsExpandX));
        gaussGroup->AddFrame(gaussFrame2, new TGLayoutHints(kLHintsExpandX));
        controlPanel->AddFrame(gaussGroup, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 5, 5, 5, 5));
        
        // 指数分布参数
        TGGroupFrame* expGroup = new TGGroupFrame(controlPanel, "Exponential Distribution");
        TGHorizontalFrame* expFrame = new TGHorizontalFrame(expGroup);
        
        TGLabel* expLambdaLabel = new TGLabel(expFrame, "Lambda (λ):");
        fExpLambdaEntry = new TGNumberEntry(expFrame, 1.0, 5, -1, TGNumberFormat::kNESReal, TGNumberFormat::kNEANonNegative);
        fExpLambdaEntry->SetLimits(TGNumberFormat::kNELLimitMinMax, 0.1, 5.0);
        
        expFrame->AddFrame(expLambdaLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 5, 2, 2));
        expFrame->AddFrame(fExpLambdaEntry, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 5, 5, 2, 2));
        
        expGroup->AddFrame(expFrame, new TGLayoutHints(kLHintsExpandX));
        controlPanel->AddFrame(expGroup, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 5, 5, 5, 5));
        
        // 泊松分布参数
        TGGroupFrame* poissonGroup = new TGGroupFrame(controlPanel, "Poisson Distribution");
        TGHorizontalFrame* poissonFrame = new TGHorizontalFrame(poissonGroup);
        
        TGLabel* poissonMeanLabel = new TGLabel(poissonFrame, "Mean (μ):");
        fPoissonMeanEntry = new TGNumberEntry(poissonFrame, 5, 5, -1, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
        fPoissonMeanEntry->SetLimits(TGNumberFormat::kNELLimitMinMax, 1, 20);
        
        poissonFrame->AddFrame(poissonMeanLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 5, 2, 2));
        poissonFrame->AddFrame(fPoissonMeanEntry, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 5, 5, 2, 2));
        
        poissonGroup->AddFrame(poissonFrame, new TGLayoutHints(kLHintsExpandX));
        controlPanel->AddFrame(poissonGroup, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 5, 5, 5, 5));
        
        // 均匀分布参数
        TGGroupFrame* uniformGroup = new TGGroupFrame(controlPanel, "Uniform Distribution");
        TGHorizontalFrame* uniformFrame1 = new TGHorizontalFrame(uniformGroup);
        TGHorizontalFrame* uniformFrame2 = new TGHorizontalFrame(uniformGroup);
        
        TGLabel* uniformMinLabel = new TGLabel(uniformFrame1, "Minimum:");
        fUniformMinEntry = new TGNumberEntry(uniformFrame1, 0.0, 5, -1, TGNumberFormat::kNESReal, TGNumberFormat::kNEAAnyNumber);
        fUniformMinEntry->SetLimits(TGNumberFormat::kNELLimitMinMax, -5.0, 5.0);
        
        TGLabel* uniformMaxLabel = new TGLabel(uniformFrame2, "Maximum:");
        fUniformMaxEntry = new TGNumberEntry(uniformFrame2, 1.0, 5, -1, TGNumberFormat::kNESReal, TGNumberFormat::kNEAAnyNumber);
        fUniformMaxEntry->SetLimits(TGNumberFormat::kNELLimitMinMax, -5.0, 5.0);
        
        uniformFrame1->AddFrame(uniformMinLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 5, 2, 2));
        uniformFrame1->AddFrame(fUniformMinEntry, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 5, 5, 2, 2));
        uniformFrame2->AddFrame(uniformMaxLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 5, 2, 2));
        uniformFrame2->AddFrame(fUniformMaxEntry, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 5, 5, 2, 2));
        
        uniformGroup->AddFrame(uniformFrame1, new TGLayoutHints(kLHintsExpandX));
        uniformGroup->AddFrame(uniformFrame2, new TGLayoutHints(kLHintsExpandX));
        controlPanel->AddFrame(uniformGroup, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 5, 5, 5, 5));
        
        // 为所有输入框连接实时更新信号
        fGaussMeanEntry->Connect("ValueSet(Long_t)", "HistogramViewer", this, "DoUpdate()");
        fGaussSigmaEntry->Connect("ValueSet(Long_t)", "HistogramViewer", this, "DoUpdate()");
        fExpLambdaEntry->Connect("ValueSet(Long_t)", "HistogramViewer", this, "DoUpdate()");
        fPoissonMeanEntry->Connect("ValueSet(Long_t)", "HistogramViewer", this, "DoUpdate()");
        fUniformMinEntry->Connect("ValueSet(Long_t)", "HistogramViewer", this, "DoUpdate()");
        fUniformMaxEntry->Connect("ValueSet(Long_t)", "HistogramViewer", this, "DoUpdate()");
        
        // 也连接文本改变信号，以便在直接输入时也能触发更新
        fGaussMeanEntry->GetNumberEntry()->Connect("TextChanged(const char*)", "HistogramViewer", this, "DoUpdate()");
        fGaussSigmaEntry->GetNumberEntry()->Connect("TextChanged(const char*)", "HistogramViewer", this, "DoUpdate()");
        fExpLambdaEntry->GetNumberEntry()->Connect("TextChanged(const char*)", "HistogramViewer", this, "DoUpdate()");
        fPoissonMeanEntry->GetNumberEntry()->Connect("TextChanged(const char*)", "HistogramViewer", this, "DoUpdate()");
        fUniformMinEntry->GetNumberEntry()->Connect("TextChanged(const char*)", "HistogramViewer", this, "DoUpdate()");
        fUniformMaxEntry->GetNumberEntry()->Connect("TextChanged(const char*)", "HistogramViewer", this, "DoUpdate()");
        
        // Exit按钮
        fExitButton = new TGTextButton(controlPanel, "&Exit");
        fExitButton->Connect("Clicked()", "TApplication", gApplication, "Terminate()");
        controlPanel->AddFrame(fExitButton, new TGLayoutHints(kLHintsCenterX | kLHintsBottom, 5, 5, 20, 10));
        
        hFrame->AddFrame(canvasFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
        hFrame->AddFrame(controlPanel, new TGLayoutHints(kLHintsRight | kLHintsExpandY));
        
        mainFrame->AddFrame(hFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
        fMain->AddFrame(mainFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
        
        fMain->MapSubwindows();
        fMain->Resize(fMain->GetDefaultSize());
        fMain->MapWindow();
    }
    
    void setupHistograms() {
        fHistograms[0] = new TH1F("hist1", "Gaussian Distribution", 100, -5, 5);
        fHistograms[1] = new TH1F("hist2", "Exponential Distribution", 100, 0, 5);
        fHistograms[2] = new TH1F("hist3", "Poisson Distribution", 20, 0, 20);
        fHistograms[3] = new TH1F("hist4", "Uniform Distribution", 100, -1.5, 2.5);
        
        fHistograms[0]->SetFillColor(kBlue);
        fHistograms[1]->SetFillColor(kRed);
        fHistograms[2]->SetFillColor(kGreen);
        fHistograms[3]->SetFillColor(kOrange);
    }
    
    void DoUpdate() {
        std::cout << "Parameter changed - updating histograms..." << std::endl;
        updateAllHistograms();
    }
    
    void updateAllHistograms() {
        // 从输入控件获取参数值
        double gaussMean = fGaussMeanEntry->GetNumber();
        double gaussSigma = fGaussSigmaEntry->GetNumber();
        double expLambda = fExpLambdaEntry->GetNumber();
        int poissonMean = (int)fPoissonMeanEntry->GetNumber();
        double uniformMin = fUniformMinEntry->GetNumber();
        double uniformMax = fUniformMaxEntry->GetNumber();
        
        // 更新直方图
        updateHistogram1(gaussMean, gaussSigma);
        updateHistogram2(expLambda);
        updateHistogram3(poissonMean);
        updateHistogram4(uniformMin, uniformMax);
        
        // 刷新所有画布
        for (int i = 0; i < 4; i++) {
            fCanvases[i]->GetCanvas()->Modified();
            fCanvases[i]->GetCanvas()->Update();
        }
        
        std::cout << "Histograms updated with new parameters!" << std::endl;
        std::cout << "Gaussian: μ=" << gaussMean << ", σ=" << gaussSigma << std::endl;
        std::cout << "Exponential: λ=" << expLambda << std::endl;
        std::cout << "Poisson: μ=" << poissonMean << std::endl;
        std::cout << "Uniform: [" << uniformMin << ", " << uniformMax << "]" << std::endl;
    }
    
    void updateHistogram1(double mean, double sigma) {
        TCanvas* canvas = fCanvases[0]->GetCanvas();
        canvas->cd();
        canvas->Clear();
        
        fHistograms[0]->Reset();
        fHistograms[0]->SetTitle(Form("Gaussian (#mu=%.2f, #sigma=%.2f)", mean, sigma));
        
        for (int i = 0; i < 10000; i++) {
            fHistograms[0]->Fill(gRandom->Gaus(mean, sigma));
        }
        
        fHistograms[0]->Draw();
    }
    
    void updateHistogram2(double lambda) {
        TCanvas* canvas = fCanvases[1]->GetCanvas();
        canvas->cd();
        canvas->Clear();
        
        fHistograms[1]->Reset();
        fHistograms[1]->SetTitle(Form("Exponential (#lambda=%.2f)", lambda));
        
        for (int i = 0; i < 10000; i++) {
            fHistograms[1]->Fill(gRandom->Exp(1.0/lambda));
        }
        
        fHistograms[1]->Draw();
    }
    
    void updateHistogram3(int mean) {
        TCanvas* canvas = fCanvases[2]->GetCanvas();
        canvas->cd();
        canvas->Clear();
        
        fHistograms[2]->Reset();
        fHistograms[2]->SetTitle(Form("Poisson (#mu=%d)", mean));
        
        for (int i = 0; i < 10000; i++) {
            fHistograms[2]->Fill(gRandom->Poisson(mean));
        }
        
        fHistograms[2]->Draw();
    }
    
    void updateHistogram4(double min, double max) {
        TCanvas* canvas = fCanvases[3]->GetCanvas();
        canvas->cd();
        canvas->Clear();
        
        fHistograms[3]->Reset();
        fHistograms[3]->SetTitle(Form("Uniform [%.2f, %.2f]", min, max));
        
        for (int i = 0; i < 10000; i++) {
            fHistograms[3]->Fill(gRandom->Uniform(min, max));
        }
        
        fHistograms[3]->Draw();
    }
};

// 在ROOT中运行的函数
void runHistogramViewer() {
    new HistogramViewer(gClient->GetRoot(), 1200, 800);
}

#ifndef __CINT__
// 独立应用程序的main函数
int main(int argc, char** argv) {
    TApplication app("Histogram Viewer", &argc, argv);
    
    std::cout << "Starting Real-time Histogram Viewer..." << std::endl;
    std::cout << "Change parameters in the input fields to see real-time updates." << std::endl;
    
    new HistogramViewer(gClient->GetRoot(), 1200, 800);
    
    app.Run();
    return 0;
}
#endif
// 在文件末尾添加这个函数
//void HistogramViewer() {
  //  new HistogramViewer(gClient->GetRoot(), 1200, 800);
//}

