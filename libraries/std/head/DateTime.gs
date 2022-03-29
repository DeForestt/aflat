class DateTime{
    private int seconds;
    private int day;
    private int month;
    private int year;
    private int hour;
    private int minute;
    private int second;

    public DateTime init(int seconds);
    public int getSeconds();
    public int getMinutes();
    public int getHours();
    public int getDays();
    public int getDay();
    public int getMonth();
    public int getYear();
    public int getSecond();
    public int getHour();
    public int getMinute();

    private int Copy<<=>>(DateTime);
    
    private int epochToDate();
};

DateTime Now(* int timeZone);