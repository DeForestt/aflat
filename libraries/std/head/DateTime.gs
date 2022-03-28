class DateTime{
    private int seconds;
    private int day;
    private int month;
    private int year;

    public DateTime init(int seconds);
    public int getSeconds();
    public int getMinutes();
    public int getHours();
    public int getDays();
    public int getDay();
    public int getMonth();
    public int getYear();
    
    private int epochToDate();
};

DateTime Now();