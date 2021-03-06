//===--- FrontendOptions.h --------------------------------------*- C++ -*-===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2017 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See https://swift.org/LICENSE.txt for license information
// See https://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//

#ifndef SWIFT_FRONTEND_FRONTENDOPTIONS_H
#define SWIFT_FRONTEND_FRONTENDOPTIONS_H

#include "swift/AST/Module.h"
#include "swift/Frontend/FrontendInputsAndOutputs.h"
#include "swift/Frontend/InputFile.h"
#include "llvm/ADT/Hashing.h"

#include <string>
#include <vector>

namespace llvm {
  class MemoryBuffer;
}

namespace swift {


/// Options for controlling the behavior of the frontend.
class FrontendOptions {
  friend class ArgsToFrontendOptionsConverter;

public:
  FrontendInputsAndOutputs InputsAndOutputs;

  /// The kind of input on which the frontend should operate.
  InputFileKind InputKind = InputFileKind::IFK_Swift;

  void forAllOutputPaths(const InputFile &input,
                         std::function<void(const std::string &)> fn) const;

  bool isOutputFileDirectory() const;

  /// A list of arbitrary modules to import and make implicitly visible.
  std::vector<std::string> ImplicitImportModuleNames;

  /// An Objective-C header to import and make implicitly visible.
  std::string ImplicitObjCHeaderPath;

  /// The name of the module which the frontend is building.
  std::string ModuleName;

  /// The path to which we should emit a serialized module.
  std::string ModuleOutputPath;

  /// The path to which we should emit a module documentation file.
  std::string ModuleDocOutputPath;

  /// The name of the library to link against when using this module.
  std::string ModuleLinkName;

  /// The path to which we should emit an Objective-C header for the module.
  std::string ObjCHeaderOutputPath;

  /// Path to a file which should contain serialized diagnostics for this
  /// frontend invocation.
  std::string SerializedDiagnosticsPath;

  /// The path to which we should output a Make-style dependencies file.
  std::string DependenciesFilePath;

  /// The path to which we should output a Swift reference dependencies file.
  std::string ReferenceDependenciesFilePath;

  /// The path to which we should output fixits as source edits.
  std::string FixitsOutputPath;

  /// The path to which we should output a loaded module trace file.
  std::string LoadedModuleTracePath;

  /// The path to which we should output a TBD file.
  std::string TBDPath;

  /// Arguments which should be passed in immediate mode.
  std::vector<std::string> ImmediateArgv;

  /// \brief A list of arguments to forward to LLVM's option processing; this
  /// should only be used for debugging and experimental features.
  std::vector<std::string> LLVMArgs;

  /// The path to output swift interface files for the compiled source files.
  std::string DumpAPIPath;

  /// The path to collect the group information for the compiled source files.
  std::string GroupInfoPath;

  /// The path to which we should store indexing data, if any.
  std::string IndexStorePath;

  /// Emit index data for imported serialized swift system modules.
  bool IndexSystemModules = false;

  /// If non-zero, warn when a function body takes longer than this many
  /// milliseconds to type-check.
  ///
  /// Intended for debugging purposes only.
  unsigned WarnLongFunctionBodies = 0;

  /// If non-zero, warn when type-checking an expression takes longer
  /// than this many milliseconds.
  ///
  /// Intended for debugging purposes only.
  unsigned WarnLongExpressionTypeChecking = 0;

  /// If non-zero, overrides the default threshold for how long we let
  /// the expression type checker run before we consider an expression
  /// too complex.
  unsigned SolverExpressionTimeThreshold = 0;

  /// The module for which we should verify all of the generic signatures.
  std::string VerifyGenericSignaturesInModule;

  enum class ActionType {
    NoneAction,        ///< No specific action
    Parse,             ///< Parse only
    Typecheck,         ///< Parse and type-check only
    DumpParse,         ///< Parse only and dump AST
    DumpInterfaceHash, ///< Parse and dump the interface token hash.
    EmitSyntax,        ///< Parse and dump Syntax tree as JSON
    DumpAST,           ///< Parse, type-check, and dump AST
    PrintAST,          ///< Parse, type-check, and pretty-print AST

    /// Parse and dump scope map.
    DumpScopeMaps,

    /// Parse, type-check, and dump type refinement context hierarchy
    DumpTypeRefinementContexts,

    EmitImportedModules, ///< Emit the modules that this one imports
    EmitPCH,             ///< Emit PCH of imported bridging header

    EmitSILGen, ///< Emit raw SIL
    EmitSIL,    ///< Emit canonical SIL

    EmitModuleOnly, ///< Emit module only
    MergeModules,   ///< Merge modules only

    EmitSIBGen, ///< Emit serialized AST + raw SIL
    EmitSIB,    ///< Emit serialized AST + canonical SIL

    Immediate, ///< Immediate mode
    REPL,      ///< REPL mode

    EmitAssembly, ///< Emit assembly
    EmitIR,       ///< Emit LLVM IR
    EmitBC,       ///< Emit LLVM BC
    EmitObject,   ///< Emit object file
  };

  bool isCreatingSIL() { return RequestedAction >= ActionType::EmitSILGen; }

  /// Indicates the action the user requested that the frontend perform.
  ActionType RequestedAction = ActionType::NoneAction;

  /// Indicates that the input(s) should be parsed as the Swift stdlib.
  bool ParseStdlib = false;

  /// If set, emitted module files will always contain options for the
  /// debugger to use.
  bool AlwaysSerializeDebuggingOptions = false;

  /// If set, dumps wall time taken to check each function body to llvm::errs().
  bool DebugTimeFunctionBodies = false;

  /// If set, dumps wall time taken to check each expression.
  bool DebugTimeExpressionTypeChecking = false;

  /// If set, prints the time taken in each major compilation phase to 
  /// llvm::errs().
  ///
  /// \sa swift::SharedTimer
  bool DebugTimeCompilation = false;

  /// The path to which we should output statistics files.
  std::string StatsOutputDir;

  /// Trace changes to stats to files in StatsOutputDir.
  bool TraceStats = false;

  /// If true, serialization encodes an extra lookup table for use in module-
  /// merging when emitting partial modules (the per-file modules in a non-WMO
  /// build).
  bool EnableSerializationNestedTypeLookupTable = true;

  /// Indicates whether or not an import statement can pick up a Swift source
  /// file (as opposed to a module file).
  bool EnableSourceImport = false;

  /// Indicates whether we are compiling for testing.
  ///
  /// \see ModuleDecl::isTestingEnabled
  bool EnableTesting = false;

  /// Enables the "fully resilient" resilience strategy.
  ///
  /// \see ResilienceStrategy::Resilient
  bool EnableResilience = false;

  /// Indicates that the frontend should emit "verbose" SIL
  /// (if asked to emit SIL).
  bool EmitVerboseSIL = false;

  /// If set, this module is part of a mixed Objective-C/Swift framework, and
  /// the Objective-C half should implicitly be visible to the Swift sources.
  bool ImportUnderlyingModule = false;

  /// If set, the header provided in ImplicitObjCHeaderPath will be rewritten
  /// by the Clang importer as part of semantic analysis.
  bool SerializeBridgingHeader = false;

  /// Indicates whether or not the frontend should print statistics upon
  /// termination.
  bool PrintStats = false;

  /// Indicates whether or not the Clang importer should print statistics upon
  /// termination.
  bool PrintClangStats = false;

  /// Indicates whether the playground transformation should be applied.
  bool PlaygroundTransform = false;
  
  /// Indicates whether the AST should be instrumented to simulate a debugger's
  /// program counter. Similar to the PlaygroundTransform, this will instrument
  /// the AST with function calls that get called when you would see a program
  /// counter move in a debugger. To adopt this implement the
  /// __builtin_pc_before and __builtin_pc_after functions.
  bool PCMacro = false;

  /// Indicates whether the playground transformation should omit
  /// instrumentation that has a high runtime performance impact.
  bool PlaygroundHighPerformance = false;

  /// Indicates whether standard help should be shown.
  bool PrintHelp = false;

  /// Indicates whether full help (including "hidden" options) should be shown.
  bool PrintHelpHidden = false;

  /// Should we sort SIL functions, vtables, witness tables, and global
  /// variables by name when we print it out. This eases diffing of SIL files.
  bool EmitSortedSIL = false;

  /// The different modes for validating TBD against the LLVM IR.
  enum class TBDValidationMode {
    None,           ///< Do no validation.
    MissingFromTBD, ///< Only check for symbols that are in IR but not TBD.
    All, ///< Check for symbols that are in IR but not TBD and TBD but not IR.
  };

  /// Compare the symbols in the IR against the TBD file we would generate.
  TBDValidationMode ValidateTBDAgainstIR = TBDValidationMode::None;

  /// The install_name to use in the TBD file.
  std::string TBDInstallName;

  /// An enum with different modes for automatically crashing at defined times.
  enum class DebugCrashMode {
    None, ///< Don't automatically crash.
    AssertAfterParse, ///< Automatically assert after parsing.
    CrashAfterParse, ///< Automatically crash after parsing.
  };

  /// Indicates a debug crash mode for the frontend.
  DebugCrashMode CrashMode = DebugCrashMode::None;

  /// Line and column for each of the locations to be probed by
  /// -dump-scope-maps.
  SmallVector<std::pair<unsigned, unsigned>, 2> DumpScopeMapLocations;

  /// Indicates whether the action will immediately run code.
  static bool isActionImmediate(ActionType);

  /// \return true if action only parses without doing other compilation steps.
  static bool shouldActionOnlyParse(ActionType);

  /// Return a hash code of any components from these options that should
  /// contribute to a Swift Bridging PCH hash.
  llvm::hash_code getPCHHashComponents() const {
    return llvm::hash_value(0);
  }

  StringRef originalPath() const;

  StringRef determineFallbackModuleName() const;

  bool isCompilingExactlyOneSwiftFile() const {
    return InputKind == InputFileKind::IFK_Swift &&
           InputsAndOutputs.hasSingleInput();
  }

private:
  bool hasUnusedDependenciesFilePath() const;
  static bool canActionEmitDependencies(ActionType);
  bool hasUnusedObjCHeaderOutputPath() const;
  static bool canActionEmitHeader(ActionType);
  bool hasUnusedLoadedModuleTracePath() const;
  static bool canActionEmitLoadedModuleTrace(ActionType);
  bool hasUnusedModuleOutputPath() const;
  static bool canActionEmitModule(ActionType);
  bool hasUnusedModuleDocOutputPath() const;
  static bool canActionEmitModuleDoc(ActionType);

public:
  static bool doesActionProduceOutput(ActionType);
  static bool doesActionProduceTextualOutput(ActionType);
  static bool needsProperModuleName(ActionType);
  static const char *suffixForPrincipalOutputFileForAction(ActionType);
};

}

#endif
