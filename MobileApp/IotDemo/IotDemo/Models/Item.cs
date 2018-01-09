namespace IotDemo.Models
{
    public class Item : BaseDataObject
    {
        string medicineName = string.Empty;
        public string Medicine
        {
            get { return medicineName; }
            set { SetProperty(ref medicineName, value); }
        }

        string name = string.Empty;
        public string Name
        {
            get { return name; }
            set { SetProperty(ref name, value); }
        }

        string doctorName = string.Empty;
        public string DoctorName
        {
            get { return doctorName; }
            set { SetProperty(ref doctorName, value); }
        }

        System.DateTime medicineAlertTime = System.DateTime.Now;

        public System.DateTime MedicineAlertTime
        {
            get { return medicineAlertTime; }
            set { SetProperty(ref medicineAlertTime, value); }
        }
    }
}
