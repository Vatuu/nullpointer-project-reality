using System.Net;
using System.Collections;
using System.ComponentModel;
using System.Diagnostics;
//Yes

using System.IO;
using System;
using System.Collections.Generic;

namespace Reality.Tools {
    class Blobinator {

        private enum TextureFormat {
            RGBA = 0,
            YUV = 1,
            CI = 2,
            IA = 3,
            I = 4
        }
    
        private struct Options {
            public string input_dir, input_meta;
            public string output_path, output_path_map;
            public string extension;
            public Type type;
        }

        private enum Type {
            TEXTURES,
            MODELS,
            SOUND,
            MUSIC
        }

        private struct DataEntry {
            public string data_id;
            public uint start, dataStart, end;
        }

        private struct TextureMetadata {
            public string fileName;
            public ushort width, height;
            public byte format, size;

            public void populate(string[] meta, string id) {
                fileName = id;
                string[] formatString = meta[1].Split('/');
                format = (byte)(TextureFormat)Enum.Parse(typeof(TextureFormat), formatString[0].ToUpper());
                size = (byte)(Math.Log(int.Parse(formatString[1]), 2) - 2);
                string[] sizeString = meta[2].Split('x');
                byte[] tw = BitConverter.GetBytes(ushort.Parse(sizeString[0]));
                Array.Reverse(tw);
                width = BitConverter.ToUInt16(tw, 0);
                byte[] th = BitConverter.GetBytes(ushort.Parse(sizeString[1]));
                Array.Reverse(th);
                height = BitConverter.ToUInt16(th, 0);
            }

            public byte[] createData() {
                using(MemoryStream stream = new MemoryStream()) {
                    using(BinaryWriter writer = new BinaryWriter(stream)) {
                        writer.Write(width); writer.Write(height);
                        writer.Write(format); writer.Write(size);
                        writer.Write((ushort)0xFF);
                        return stream.ToArray();
                    }
                }
            }
        }

        static void Main(string[] args) {
            var options = ParseOptions(args);

            if(options.type == Type.TEXTURES) {
                Console.WriteLine("\nData Mode: TEXTURES\n");
                Dictionary<string, TextureMetadata> data = GetTextureData(options);
                foreach(KeyValuePair<string, TextureMetadata> entry in data) {
                    Console.WriteLine("DataNode: {0}", entry.Key);
                    Console.WriteLine("  {0} {1}/{2} {3}x{4}", entry.Value.fileName, entry.Value.format, entry.Value.size, entry.Value.width, entry.Value.height);
                }
                List<DataEntry> metadata = CreateBlobDataTexture(data, options.output_path);
                CreateLookupTableTexture(metadata, options.output_path_map);
            }

            Console.WriteLine("Done!");
        }

        private static Options ParseOptions(string[] args) {
            Options option = new Options();

            for (int i = 0; i < args.Length; i++)
            {
                string arg = args[i];

                switch(arg) {
                    case "-i":
                    case "-input":
                        option.input_dir = args[++i];
                        break;
                    case "-d":
                    case "-data":
                        option.input_meta = args[++i];
                        break;
                    case "-o":
                    case "-output":
                        option.output_path = args[++i];
                        break;
                    case "-m":
                    case "-map":
                        option.output_path_map = args[++i];
                        break;
                    case "-t":
                    case "-type":
                        option.type = (Type)Enum.Parse(typeof(Type), args[++i]);
                        break;
                    case "-e":
                    case "-extension":
                        option.extension = args[++i];
                        break;
                    case "-h":
                    case "-?":
                    case "-help":
                    case "--help":
                        PrintUsage();
                        Environment.Exit(0);
                        break;
                    default:
                        Console.WriteLine("Unknown Option: " + arg);
                        break;
                        
                }
            }

            return option;
        }

        private static void PrintUsage() {
            Console.WriteLine("Blobinator {0} by Nicolas \"Vatuu\" Adamoglou", "1.0");
            Console.WriteLine("  A tool to package data assets and generate a identifier-based");
            Console.WriteLine("  C file for accessing data without static definitions.\n");
            Console.WriteLine("Options:");
            Console.WriteLine("  -i -include");
            Console.WriteLine("     Directory containing the files to be included.");
            Console.WriteLine("  -t -type [TEXTURE|MODEL|SOUND]");
            Console.WriteLine("     Defines the data type to be blobbed.\n     Will be used by the generated C template.");
            Console.WriteLine("  -o -output");
            Console.WriteLine("     Path at which the blobfish will be generated.");
            Console.WriteLine("  -e -extension");
            Console.WriteLine("     Extension of the files to be blobbed.");
            Console.WriteLine("  -m -map");
            Console.WriteLine("     Path at which the C file will be generated.");
            Console.WriteLine("  -d -data");
            Console.WriteLine("     Path to the texture metadata map (TEXTURES only).");
            Console.WriteLine("  -h -? -help");
            Console.WriteLine("     Prints this message.");
        }
    
        private static List<string> GetDataFiles(string directory, string extension) {
            List<string> files = new List<string>();
            try {
                foreach(string f in Directory.GetFiles(directory)) {
                    if(f.EndsWith(extension))
                        files.Add(f);
                }

                foreach(string d in Directory.GetDirectories(directory))
                    files.AddRange(GetDataFiles(d, extension));
            } catch(System.Exception except) {
                Console.WriteLine("Folder filter error!\n" + except.ToString());
            } 
            return files;
        }
    
        #region "Textures"

        private static Dictionary<string, TextureMetadata> GetTextureData(Options option) {
            Dictionary<string, TextureMetadata> data = new Dictionary<string, TextureMetadata>();
            List<string> files = GetDataFiles(option.input_dir, option.extension);

            try {
                string contentsMaps = File.ReadAllText(option.input_meta);
                List<string> mapData = new List<string>(contentsMaps.Split('\n'));
                mapData.ForEach(d => {
                    string[] meta = d.Split(' ');
                    if(files.Contains(meta[0])) {
                        string id = meta[0].Remove(0, option.input_dir.Length).Split('.')[0];
                        if(id.StartsWith("/"))
                            id = id.Remove(0, 1);
                        TextureMetadata tex = new TextureMetadata();
                        tex.populate(meta, id);
                        data.Add(meta[0], tex);
                    }
                });

                return data;
            } catch(System.Exception except) {
                Console.WriteLine("Texheader creation error!\n" + except.ToString());
            }

            return null;
        }

        private static List<DataEntry> CreateBlobDataTexture(Dictionary<string, TextureMetadata> data, string output) {
            using(MemoryStream stream = new MemoryStream()) {
                using(BinaryWriter writer = new BinaryWriter(stream)) {
                    List<DataEntry> dataEntries = new List<DataEntry>();

                    foreach(KeyValuePair<string, TextureMetadata> entry in data) {
                        DataEntry info = new DataEntry();
                        info.data_id = entry.Value.fileName;
                        byte[] header = entry.Value.createData();
                        info.start = (uint)stream.Length;
                        writer.Write(header);
                        info.dataStart = (uint)stream.Length;
                        try{
                            byte[] data_buffer = File.ReadAllBytes(entry.Key);
                            writer.Write(data_buffer);
                        }catch(System.Exception except) {
                            Console.WriteLine("Failed to read datafile! " + except.ToString());
                        }

                        info.end = (uint)stream.Length;
                        dataEntries.Add(info);
                    }

                    using(BinaryWriter outstream = new BinaryWriter(new FileStream(output, FileMode.Create))) {
                        try {
                            outstream.Write(stream.ToArray());
                        } catch(System.Exception except) {
                            Console.WriteLine("Failed to write blob! " + except.ToString());
                        }
                    }

                    return dataEntries;
                }
            }
        }

        private static void CreateLookupTableTexture(List<DataEntry> data, string map) {
            List<string> texture_template = new List<string>{
                "#include <textures.h>\n",
                "const size_t TEXTURE_COUNT = {0};\n",
                "data_info TEXTURES[] = {",
                "};\n"
            };

            string centryLine = "   {{ \"{0}\", {1}, {2}, {3}, NULL }},"; 

            using(StreamWriter writer = new StreamWriter(new FileStream(map, FileMode.Create)))  {
                try {
                    writer.WriteLine(texture_template[0]);
                    writer.WriteLine(String.Format(texture_template[1], data.Count));
                    writer.WriteLine(texture_template[2]);

                    data.ForEach(l => writer.WriteLine(String.Format(centryLine, l.data_id, l.start, l.dataStart, l.end)));

                    writer.WriteLine(texture_template[3]);
                } catch(System.Exception except) {
                    Console.WriteLine("Failed writing C Map! " + except.ToString());
                }
            }
        }

        #endregion
    }
}
