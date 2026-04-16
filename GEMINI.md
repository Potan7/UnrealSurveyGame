# Project Overview: Survey

This is an Unreal Engine 5.7 C++ project designed as an immersive "Survey" simulation. The core experience involves a player character interacting with a physical 3D computer setup, where 3D mouse movements are realistically mapped to an on-screen 2D cursor for survey interaction.

## Key Features
- **Immersive Interaction**: 3D mouse model movement mapped to 2D UI coordinates.
- **Hand IK**: Procedural hand animation tracking the 3D mouse actor.
- **Data-Driven Survey**: Questions and events are driven by a `UDataTable` (`FSurveyData`).
- **Localization Support**: Uses Unreal Engine String Tables for multilingual support.
- **Enhanced Input**: Utilizes the modern Enhanced Input system for player controls.
- **Custom Widget Interaction**: Employs `UWidgetInteractionComponent` with manual hit result injection for interacting with 3D-placed UI.

## Architecture & Core Modules
- **`ASurveyCharacter`**: The main pawn controlling the mouse interaction, coordinate mapping, and Hand IK updates.
- **`USurveyMonitorWidget`**: The UI component managing the survey flow, question display, and event triggers.
- **`FSurveyData`**: A struct defining survey rows (Question, EventTrigger, EventDelay).
- **`USurveyCharacterAnimInstance`**: Handles IK targets for the character's hands.

## Building and Running
1.  **Engine Version**: Unreal Engine 5.7 (or compatible custom association).
2.  **Generate Project Files**: Right-click `Survey.uproject` and select "Generate Visual Studio project files".
3.  **Build**: Open `Survey.sln` in Visual Studio or Rider and build the `Survey` project for `Development Editor`.
4.  **Run**: Launch the editor and play the `Main` or `Main1` level.
5.  **Dependencies**: Requires `UMG`, `EnhancedInput`, and `InputCore` modules (defined in `Survey.Build.cs`).

## Development Conventions
- **Naming Conventions**: Strictly follow Unreal Engine's coding standards:
    - `A` prefix for Actors (e.g., `ASurveyCharacter`).
    - `U` prefix for Objects/Components (e.g., `USurveyMonitorWidget`).
    - `F` prefix for Structs (e.g., `FSurveyData`).
    - `PascalCase` for all variables and functions.
- **UI Logic**: Prefer `NativeConstruct` and C++ for logic in widgets, with `BindWidget` for UI components.
- **Data-Driven Design**: New survey content should be added via the CSV files in `Content/Data/csv/` and re-imported into the corresponding `UDataTable`.
- **Localization**: All display text should reside in String Tables (e.g., `/Game/Data/ST_Question`).
- **Input**: Map new actions through the Enhanced Input system (`IMC_Default`).

## Key Files
- `Survey.uproject`: Project manifest and engine association.
- `Source/Survey/SurveyCharacter.h/cpp`: Core interaction and mouse-to-UI mapping logic.
- `Source/Survey/SurveyMonitorWidget.h/cpp`: Survey flow and UI management.
- `Source/Survey/SurveyCharacterAnimInstance.h`: Hand IK target definitions.
- `Content/Data/csv/survey.csv`: The source for the survey data table.
- `Config/DefaultInput.ini`: Enhanced Input configurations.
