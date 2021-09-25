using Microsoft.EntityFrameworkCore.Migrations;

namespace SunBatherAPI.Migrations
{
    public partial class AddManualPumpOn : Migration
    {
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AlterColumn<float>(
                name: "SetTemperature",
                table: "SystemStatus",
                type: "float",
                nullable: false,
                oldClrType: typeof(double),
                oldType: "double");

            migrationBuilder.AddColumn<bool>(
                name: "ManualPumpOn",
                table: "SystemStatus",
                type: "tinyint(1)",
                nullable: false,
                defaultValue: false);
        }

        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "ManualPumpOn",
                table: "SystemStatus");

            migrationBuilder.AlterColumn<double>(
                name: "SetTemperature",
                table: "SystemStatus",
                type: "double",
                nullable: false,
                oldClrType: typeof(float),
                oldType: "float");
        }
    }
}
