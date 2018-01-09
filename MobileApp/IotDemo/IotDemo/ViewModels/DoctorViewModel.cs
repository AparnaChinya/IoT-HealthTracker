using System;
using System.Windows.Input;
using Xamarin.Forms;

namespace IotDemo.ViewModels
{
	public class DoctorViewModel : BaseViewModel
	{
		public DoctorViewModel()
		{
			Title = "Doctor";

			OpenWebCommand = new Command(() => Device.OpenUri(new Uri("https://xamarin.com/platform")));
		}

		/// <summary>
		/// Command to open browser to xamarin.com
		/// </summary>
		public ICommand OpenWebCommand { get; }
	}
}
