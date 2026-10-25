object TqztrNative {
    @JvmStatic external fun showMessage(msg: String)
    @JvmStatic external fun currentFilePath(): String
    @JvmStatic external fun addDiagnostic(line: Int, col: Int, severity: Int, msg: String)
    @JvmStatic external fun clearDiagnostics()
    @JvmStatic external fun registerAction(menu: String, label: String, handlerClass: String)
}
