// <auto-generated />
using System;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Infrastructure;
using Microsoft.EntityFrameworkCore.Migrations;
using Microsoft.EntityFrameworkCore.Storage.ValueConversion;
using SunBatherAPI.Data;

namespace SunBatherAPI.Migrations
{
    [DbContext(typeof(DataContext))]
    [Migration("20210830051541_InitialCreate")]
    partial class InitialCreate
    {
        protected override void BuildTargetModel(ModelBuilder modelBuilder)
        {
#pragma warning disable 612, 618
            modelBuilder
                .HasAnnotation("Relational:MaxIdentifierLength", 64)
                .HasAnnotation("ProductVersion", "5.0.8");

            modelBuilder.Entity("SunBatherAPI.Models.RecordEvent", b =>
                {
                    b.Property<int>("Id")
                        .ValueGeneratedOnAdd()
                        .HasColumnType("int");

                    b.Property<double>("Cost")
                        .HasColumnType("double");

                    b.Property<double>("Emissions")
                        .HasColumnType("double");

                    b.Property<double>("EnergyAdsorbed")
                        .HasColumnType("double");

                    b.Property<DateTime>("ReadDateTime")
                        .HasColumnType("datetime");

                    b.Property<double>("SolarIrradiance")
                        .HasColumnType("double");

                    b.Property<int>("SystemIdentityID")
                        .HasColumnType("int");

                    b.Property<double>("TemperatureValueInput")
                        .HasColumnType("double");

                    b.Property<double>("TemperatureValueOutput")
                        .HasColumnType("double");

                    b.Property<double>("TemperatureValueRoof")
                        .HasColumnType("double");

                    b.HasKey("Id");

                    b.HasIndex("SystemIdentityID");

                    b.ToTable("RecordEvent");
                });

            modelBuilder.Entity("SunBatherAPI.Models.SystemIdentity", b =>
                {
                    b.Property<int>("Id")
                        .ValueGeneratedOnAdd()
                        .HasColumnType("int");

                    b.Property<byte[]>("ProductId")
                        .IsRequired()
                        .HasColumnType("varbinary(16)");

                    b.HasKey("Id");

                    b.ToTable("SystemID");
                });

            modelBuilder.Entity("SunBatherAPI.Models.SystemStatus", b =>
                {
                    b.Property<int>("Id")
                        .ValueGeneratedOnAdd()
                        .HasColumnType("int");

                    b.Property<bool>("PumpMode")
                        .HasColumnType("tinyint(1)");

                    b.Property<bool>("PumpStatus")
                        .HasColumnType("tinyint(1)");

                    b.Property<double>("SetTemperature")
                        .HasColumnType("double");

                    b.Property<int>("SystemIdentityID")
                        .HasColumnType("int");

                    b.HasKey("Id");

                    b.HasIndex("SystemIdentityID");

                    b.ToTable("SystemStatus");
                });

            modelBuilder.Entity("SunBatherAPI.Models.TemperatureSensor", b =>
                {
                    b.Property<int>("Id")
                        .ValueGeneratedOnAdd()
                        .HasColumnType("int");

                    b.Property<DateTime>("ReadDate")
                        .HasColumnType("datetime");

                    b.Property<double>("Value")
                        .HasColumnType("double");

                    b.HasKey("Id");

                    b.ToTable("TemperatureSensor");
                });

            modelBuilder.Entity("SunBatherAPI.Models.RecordEvent", b =>
                {
                    b.HasOne("SunBatherAPI.Models.SystemIdentity", "SystemIdentity")
                        .WithMany()
                        .HasForeignKey("SystemIdentityID")
                        .OnDelete(DeleteBehavior.Cascade)
                        .IsRequired();

                    b.Navigation("SystemIdentity");
                });

            modelBuilder.Entity("SunBatherAPI.Models.SystemStatus", b =>
                {
                    b.HasOne("SunBatherAPI.Models.SystemIdentity", "SystemIdentity")
                        .WithMany()
                        .HasForeignKey("SystemIdentityID")
                        .OnDelete(DeleteBehavior.Cascade)
                        .IsRequired();

                    b.Navigation("SystemIdentity");
                });
#pragma warning restore 612, 618
        }
    }
}
