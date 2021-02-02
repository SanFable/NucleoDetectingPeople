
class CSensorBase
{
    public:
    CSensorBase( )//CDataHandler* DataHandlerPtr) : poDataHandler(DataHandlerPtr)
    {
    }
    ~CSensorBase();

    virtual int Init();
    virtual int GetData();
    virtual int SetDataToDataHandler();
    //jakies inne metody

   // private:
    //CDataHandler* poDataHandler;

};