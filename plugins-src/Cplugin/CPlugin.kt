object CPlugin {
    @JvmStatic
    fun onLoad() {
        TqztrNative.registerAction("Tools", "Analyze C/C++ File", "CPlugin")
    }

    @JvmStatic
    fun onInvoke() {
        val path = TqztrNative.currentFilePath()
        if (path.isEmpty()) {
            TqztrNative.showMessage("Najpierw otwórz jakiś plik C/C++.")
            return
        }

        TqztrNative.clearDiagnostics()

        val cmd = listOf(
            "clang", "-fsyntax-only", "-Wall", "-Wextra",
            "-fno-color-diagnostics", path
        )

        val proc = try {
            ProcessBuilder(cmd).redirectErrorStream(true).start()
        } catch (e: Exception) {
            TqztrNative.showMessage("Nie znaleziono clang w PATH.")
            return
        }

        val output = proc.inputStream.bufferedReader().readLines()
        proc.waitFor()

        val regex = Regex("""^(.+?):(\d+):(\d+):\s+(error|warning|note):\s+(.+)$""")
        var errs = 0
        var warns = 0

        for (line in output) {
            val m = regex.find(line) ?: continue
            val ln = m.groupValues[2].toInt() - 1
            val col = m.groupValues[3].toInt() - 1
            val sev = when (m.groupValues[4]) {
                "error" -> 2
                "warning" -> 1
                else -> 0
            }
            val msg = m.groupValues[5]

            TqztrNative.addDiagnostic(ln, col, sev, msg)
            if (sev == 2) errs++
                if (sev == 1) warns++
        }

        if (errs == 0 && warns == 0) TqztrNative.showMessage("Czysto! 0 błędów, 0 ostrzeżeń.")
            else TqztrNative.showMessage("Znaleziono: $errs błędów, $warns ostrzeżeń.")
    }
}
