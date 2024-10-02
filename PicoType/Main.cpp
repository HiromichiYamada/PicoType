# include <Siv3D.hpp>

enum class AppState {
	Title,
	Countdown,
	Typing,
	Result,
};

struct App
{
	Font font{ FontMethod::MSDF, 48, Typeface::Bold };

	AppState state = AppState::Title;

	// カウントダウン.
	double countTime = 0;
	int Count = 3;

	// 問題文リスト.
	std::vector<std::pair<String, String>> questions =
	{
		{ U"ピコテラス", U"pikoterasu" },
		{ U"ピコテスラ", U"pikotesura" },
		{ U"ピコステラ", U"pikosutera" },
		{ U"ピコスラテ", U"pikosurate" },
		{ U"ピコラテス", U"pikoratesu" },

		/*
		{ U"ピコラステ", U"pikorasute" },
		{ U"ピテラスコ", U"piterasuko" },
		{ U"ピラスコテ", U"pirasukote" },
		{ U"ピスコテラ", U"pisukotera" },
		{ U"コピステラ", U"kopisutera" },

		{ U"コテピラス", U"kotepirasu" },
		{ U"コテラスピ", U"koterasupi" },
		{ U"コラピステ", U"korapisute" },
		{ U"テラスピコ", U"terasupiko" },
		{ U"テピコスラ", U"tepikosura" },

		{ U"スピコテラ", U"supikotera" },
		{ U"スコテラピ", U"sukoterapi" },
		{ U"スラテコピ", U"suratekopi" },
		//{ U"ピピテラス", U"pipiterasu" },
		{ U"ススススス", U"sususususu" },
		{ U"テララテラ", U"teraratera" },
		*/
	};
	Array<int> questionIndices;	// 表示順に問題インデックスを格納する配列.
	int questionIndex = -1;		// 表示中の問題インデックス.
	int nextQuestion = 0;	// 次の問題の順番.

	// リザルト関係.
	int NumQuestions = 0;
	int NumMissType = 0;
	double TimeFinished = 0;
	bool FlagMissType = false;


	// 問題文(表示用).
	Optional<String> questionShow;

	// 問題文の文字列.
	String target;

	// 入力中の文字列
	String input;

	// サウンド.
	Audio audioOk01{ U"se/ok-01.mp3" };
	Audio audioOk22{ U"se/ok-22.mp3" };
	Audio audioError05{ U"se/error-05.mp3" };
	Audio audioFinished{ U"se/finished.mp3" };
	void shotAudio(Audio& audio) {
		if (audio.isPlaying()) {
			audio.stop();
		}
		audio.play();
	}

	// 問題の順番を設定.
	void initQuestions() {
		questionIndices.clear();
		for (int i = 1; i < questions.size(); i++)	// [1]..[num-1]を設定.
		{
			questionIndices.push_back(i);
		}
		questionIndices.shuffle();		// [1]..[num-1]をシャッフル.
		questionIndices.push_front(0);	// 1問目は0に固定.
		nextQuestion = 0;
		Logger << questionIndices;

		NumQuestions = questions.size();
		NumMissType = 0;
		TimeFinished = 0;
	}
	String getQuestionLabelAt(int i) {
		return questions[i].first;
	}
	String getQuestionInputAt(int i) {
		return questions[i].second;
	}

	/// @brief 次の問題を設定.
	/// @return false:問題設定できませんでした→終了.
	bool setNextQuestion()
	{
		if (nextQuestion < questionIndices.size()) {
			questionIndex = questionIndices[nextQuestion];

			questionShow = getQuestionLabelAt(questionIndex);
			target = getQuestionInputAt(questionIndex);
			input.clear();

			nextQuestion++;

			shotAudio(audioOk22);

			return true;
		}
		else {
			return false;
		}
	}

	const Size WindowSize{ 1000, 600 };
	Color BackgroundColor{ 160, 160, 160 };
	Image image{ WindowSize, BackgroundColor };
	DynamicTexture texture{ image };
};


bool Button(const Rect& rect, const Font& font, const String& text, bool enabled)
{
	if (enabled && rect.mouseOver())
	{
		Cursor::RequestStyle(CursorStyle::Hand);
	}

	if (enabled)
	{
		rect.draw(ColorF{ 0.3, 0.7, 1.0 });
		font(text).drawAt(40, (rect.x + rect.w / 2), (rect.y + rect.h / 2));
	}
	else
	{
		rect.draw(ColorF{ 0.5 });
		font(text).drawAt(40, (rect.x + rect.w / 2), (rect.y + rect.h / 2), ColorF{ 0.7 });
	}

	return (enabled && rect.leftClicked());
}

void StartCountdown(App& app)
{
	app.questionIndex = -1;
	app.nextQuestion = 0;

	app.NumMissType = 0;
	double TimeFinished = 0;
	app.FlagMissType = false;

	app.countTime = 0;
	app.Count = 3;

	app.state = AppState::Countdown;
	app.shotAudio(app.audioOk22);

	app.image.clear();
	app.image = Image(app.WindowSize, app.BackgroundColor);
}


void UpdateTitle(App& app)
{
	app.font(U"PicoType").drawBaseAt(64, Scene::Width() * 0.5, Scene::Height() * 0.5 - 80, ColorF{ 1.0 });
	app.font(U"PRESS SPACE TO START").drawBaseAt(24, Scene::Width() * 0.5, Scene::Height() * 0.5 + 100, ColorF{ 0.98 });

	// スペースキーが押されたら
	if (KeySpace.down())
	{
		StartCountdown(app);
	}
	// A キーが押されたら
	if (KeyA.down())
	{
		Print << U"A";
	}
}

void UpdateCountdown(App& app)
{
	app.countTime += Scene::DeltaTime();
	if (app.countTime >= 1.0) {
		app.Count--;
		if (app.Count == 2) {
			app.countTime -= 1.0;
			app.shotAudio(app.audioOk01);
		}
		else if (app.Count == 1) {
			app.countTime -= 1.0;
			app.shotAudio(app.audioOk01);
		}
		else if (app.Count == 0) {
			// Not implemented yet.
			app.state = AppState::Typing;
			//app.countTime -= 1.0;
			app.shotAudio(app.audioOk22);
			return;
		}
	}
	app.font(Format(app.Count)).drawBaseAt(120, Scene::Width() * 0.5, Scene::Height() * 0.5 + 20, ColorF{ 1.0 });
}

void UpdateTyping(App& app)
{
	app.TimeFinished += Scene::DeltaTime();

	int lengthPreInput = app.input.size();

	// テキスト入力（TextInputMode::DenyControl: エンターやタブ、バックスペースは受け付けない）
	TextInput::UpdateText(app.input, TextInputMode::DenyControl);

	// 誤った入力が含まれていたら削除する
	while (not app.target.starts_with(app.input))
	{
		app.input.pop_back();
		app.shotAudio(app.audioError05);
		app.NumMissType++;
		app.FlagMissType = true;
	}

	int lengthPostInput = app.input.size();

	if (lengthPreInput != lengthPostInput) {
		// 1文字入力できた.
		app.shotAudio(app.audioOk01);
		app.FlagMissType = false;
	}

	bool isTyping = true;
	// 一致したら次の問題へ移る
	if (!app.questionShow) {
		// 問題準備.
		isTyping = app.setNextQuestion();
	}
	else if (app.input == app.target)
	{
		// 問題準備.
		isTyping = app.setNextQuestion();
	}
	else {
		// 問題回答中.
		isTyping = true;
	}

	if (isTyping) {
		if (app.questionShow) {
			double CenterX = Scene::Width() * 0.5;
			double CenterY = Scene::Height() * 0.5;
			// 問題文を描画する
			app.font(app.questionShow.value()).drawBaseAt(70, Vec2{ CenterX, CenterY - 80 }, ColorF{ 0.98 });
			// 問題文を描画する
			app.font(app.target).drawBaseAt(50, Vec2{ CenterX, CenterY }, ColorF{ 0.98 });
			// 入力中の文字を描画する
			app.font(app.input).drawBaseAt(50, Vec2{ CenterX, CenterY + 100 }, ColorF{ 0.12 });
		}
	}
	else {
		// 終了！.
		app.state = AppState::Result;
		app.shotAudio(app.audioFinished);
	}

}

void UpdateResult(App& app)
{
	double X0 = 40;
	double Y0 = 100;
	double Y1 = 220;
	double Y2 = 300;
	app.font(U"終了！").draw(60, Vec2{ X0, Y0 }, ColorF{ 0.12 });
	app.font(U"クリアタイム:").draw(40, Vec2{ X0, Y1 }, ColorF{ 0.12 });
	app.font(U"{:.2f}秒"_fmt(app.TimeFinished)).draw(40, Vec2{ X0 + 350, Y1 }, ColorF{ 0.12 });
	app.font(U"ミスタイプ:").draw(40, Vec2{ X0, Y2 }, ColorF{ 0.12 });
	app.font(U"{0}回"_fmt(app.NumMissType)).draw(40, Vec2{ X0 + 350, Y2 }, ColorF{ 0.12 });

	if (Button(Rect{ 600, 500, 350, 50 }, app.font, U"タイトルにもどる", true))
	{
		app.state = AppState::Title;
	}
}

void DrawPicoTypeBG(App& app)
{
	int numChars = app.target.length();
	if (numChars <= 0) {
		return;
	}
	int curCharAt = app.input.length();

	int CellWidth = Scene::Width() / app.NumQuestions;
	int CellHeight = Scene::Height() / numChars;

	Color CellColor{ 150, 150, 200, 200 };	// Correct Color.
	if (app.FlagMissType) {
		CellColor = Color(200, 150, 150, 200);	// Miss Color.
	}

	int CellX0 = (app.nextQuestion - 1) * CellWidth;
	int isLast = (app.NumQuestions == app.nextQuestion);
	if (isLast) {
		CellWidth = Scene::Width() - CellX0;
	}
	Rect(CellX0, curCharAt * CellHeight, CellWidth, CellHeight).overwrite(app.image, CellColor);
	app.texture.fillIfNotBusy(app.image);
	app.texture.draw();
}


void Main()
{
	App app;

	Window::Resize(app.WindowSize.x, app.WindowSize.y);
	Window::SetTitle(U"PicoType ver 0.1");


	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	app.initQuestions();

	while (System::Update())
	{
		switch (app.state)
		{
		case AppState::Title:
			UpdateTitle(app);
			break;
		case AppState::Countdown:
			UpdateCountdown(app);
			break;
		case AppState::Typing:
			DrawPicoTypeBG(app);
			UpdateTyping(app);
			break;
		case AppState::Result:
			DrawPicoTypeBG(app);
			UpdateResult(app);
			break;
		default:
			break;
		}
	}
}
