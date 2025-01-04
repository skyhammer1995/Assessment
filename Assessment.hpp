/* initial thoughts reading the PDF*/
// inputs:
// print name STRING?
// destination output folder  STRING?
// mode Boolean? 
// I think I will keep this in just the Assessment documents 
// and consolidate things to the FakePrinter class for now, normally I would think about organizing this into more dedicated classes for behaviors.
// I have not read in .csv data before, so that will be a new challenge for me, but I have worked with other I/O so I'm expecting I will read up on best practices for it and it should be similar to other input patterns. 
// I've also not directly worked with any kind of keyboard interrupt, but my thinking is something with maybe a try-catch or if there's a way to assign a signal to the keystroke to act as interrupt, I'll try that.

// I will try to comment in a more verbose manner, rather than what I normally would do for clean commenting hopefully to help with the interview process.

#include <string>
#include <unordered_map>
#include <vector>

// I want this for readability rather than using a true/false for the modes. In the future we could refactor if we wanted more modes, and could change the data type used for `m_mode`
enum class PrintMode {
    Automatic,
    Supervised
};

// I'd like this to be more robust with regex or some other method maybe, but for now this is what I can think of
std::unordered_map<std::string, PrintMode> modeMap = {
    {"automatic", PrintMode::Automatic},
    {"supervised", PrintMode::Supervised}
};

class FakePrinter {
public:
    FakePrinter() = delete;
    explicit FakePrinter(const std::string& printName,
                         const std::string& outputPath,
                         const PrintMode& printMode);
    ~FakePrinter() = default;
    FakePrinter(const FakePrinter&) = delete;
    FakePrinter& operator=(const FakePrinter&) = delete;

    // CSV reading method
    void loadFromFile(const std::string& filename);
    std::vector<std::vector<std::string>> m_csvData;

    // print functionality
    void print();

private:
    // print handling
    void handleAutomaticPrint();
    void handleSupervisedPrint();

    // create output file
    void createOutputFile();

    // input members
    std::string m_printName;
    std::string m_outputPath;
    PrintMode m_printMode;
};