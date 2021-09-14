using System;
using Microsoft.EntityFrameworkCore.Migrations;
using MySql.EntityFrameworkCore.Metadata;

namespace SunBatherAPI.Migrations
{
    public partial class InitialCreate : Migration
    {
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.CreateTable(
                name: "SystemID",
                columns: table => new
                {
                    Id = table.Column<int>(type: "int", nullable: false)
                        .Annotation("MySQL:ValueGenerationStrategy", MySQLValueGenerationStrategy.IdentityColumn),
                    ProductId = table.Column<byte[]>(type: "varbinary(16)", nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_SystemID", x => x.Id);
                });

            migrationBuilder.CreateTable(
                name: "TemperatureSensor",
                columns: table => new
                {
                    Id = table.Column<int>(type: "int", nullable: false)
                        .Annotation("MySQL:ValueGenerationStrategy", MySQLValueGenerationStrategy.IdentityColumn),
                    Value = table.Column<double>(type: "double", nullable: false),
                    ReadDate = table.Column<DateTime>(type: "datetime", nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_TemperatureSensor", x => x.Id);
                });

            migrationBuilder.CreateTable(
                name: "RecordEvent",
                columns: table => new
                {
                    Id = table.Column<int>(type: "int", nullable: false)
                        .Annotation("MySQL:ValueGenerationStrategy", MySQLValueGenerationStrategy.IdentityColumn),
                    SystemIdentityID = table.Column<int>(type: "int", nullable: false),
                    TemperatureValueInput = table.Column<double>(type: "double", nullable: false),
                    TemperatureValueOutput = table.Column<double>(type: "double", nullable: false),
                    TemperatureValueRoof = table.Column<double>(type: "double", nullable: false),
                    SolarIrradiance = table.Column<double>(type: "double", nullable: false),
                    Cost = table.Column<double>(type: "double", nullable: false),
                    Emissions = table.Column<double>(type: "double", nullable: false),
                    EnergyAdsorbed = table.Column<double>(type: "double", nullable: false),
                    ReadDateTime = table.Column<DateTime>(type: "datetime", nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_RecordEvent", x => x.Id);
                    table.ForeignKey(
                        name: "FK_RecordEvent_SystemID_SystemIdentityID",
                        column: x => x.SystemIdentityID,
                        principalTable: "SystemID",
                        principalColumn: "Id",
                        onDelete: ReferentialAction.Cascade);
                });

            migrationBuilder.CreateTable(
                name: "SystemStatus",
                columns: table => new
                {
                    Id = table.Column<int>(type: "int", nullable: false)
                        .Annotation("MySQL:ValueGenerationStrategy", MySQLValueGenerationStrategy.IdentityColumn),
                    SystemIdentityID = table.Column<int>(type: "int", nullable: false),
                    PumpMode = table.Column<bool>(type: "tinyint(1)", nullable: false),
                    SetTemperature = table.Column<double>(type: "double", nullable: false),
                    PumpStatus = table.Column<bool>(type: "tinyint(1)", nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_SystemStatus", x => x.Id);
                    table.ForeignKey(
                        name: "FK_SystemStatus_SystemID_SystemIdentityID",
                        column: x => x.SystemIdentityID,
                        principalTable: "SystemID",
                        principalColumn: "Id",
                        onDelete: ReferentialAction.Cascade);
                });

            migrationBuilder.CreateIndex(
                name: "IX_RecordEvent_SystemIdentityID",
                table: "RecordEvent",
                column: "SystemIdentityID");

            migrationBuilder.CreateIndex(
                name: "IX_SystemStatus_SystemIdentityID",
                table: "SystemStatus",
                column: "SystemIdentityID");
        }

        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropTable(
                name: "RecordEvent");

            migrationBuilder.DropTable(
                name: "SystemStatus");

            migrationBuilder.DropTable(
                name: "TemperatureSensor");

            migrationBuilder.DropTable(
                name: "SystemID");
        }
    }
}
