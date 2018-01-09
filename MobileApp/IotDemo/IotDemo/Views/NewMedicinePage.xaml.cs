using System;

using IotDemo.Models;

using Xamarin.Forms;

namespace IotDemo.Views
{
	public partial class NewMedicinePage : ContentPage
	{
		public Item Item { get; set; }

		public NewMedicinePage()
		{
			InitializeComponent();

            Item = new Item
            {
                Name = "Phil Dunphy",
                DoctorName = "Claire",
                Medicine = "Vicks",
                MedicineAlertTime = System.DateTime.Now

			};

			BindingContext = this;
		}

		async void Save_Clicked(object sender, EventArgs e)
		{
			MessagingCenter.Send(this, "AddItem", Item);
			await Navigation.PopToRootAsync();
		}
	}
}