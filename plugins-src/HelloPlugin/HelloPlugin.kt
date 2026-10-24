// Plik: plugins-src/HelloPlugin/HelloPlugin.kt
// Kompilacja: kotlinc HelloPlugin.kt -include-runtime -d HelloPlugin.jar

object HelloPlugin {
    @JvmStatic
    fun onLoad() {
        // Wywołanie natywnej metody C++ (zarejestrowanej przez JNI)
        nativeShowMessage("Witaj z wtyczki Kotlin w TQZTR Editor!")
        nativeInsertText("// Ten kod został wstawiony przez wtyczkę Kotlin\n")
    }

    // Deklaracje natywne — C++ je implementuje
    @JvmStatic private external fun nativeShowMessage(msg: String)
    @JvmStatic private external fun nativeInsertText(text: String)
}