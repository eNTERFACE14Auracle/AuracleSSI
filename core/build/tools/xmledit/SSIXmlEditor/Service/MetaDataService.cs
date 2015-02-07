using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace SSIXmlEditor.Service
{
	[Serializable()]
	class MetaDataCache
	{
		public DateTime Time;
		public XMLReader.MetaData Data;

		public override string ToString()
		{
			return Data.Lib;
		}
	}

    class MetaDataService : IMetaDataService
    {
        private string Path { get; set; }
        private List<MetaData> m_MetaDatas;

        public MetaDataService(string path)
        {
            if (string.IsNullOrEmpty(path))
                throw new ArgumentNullException(path);

            Path = path;
        }

        #region IMetaDataService Members

        public IEnumerable<MetaData> MetaDatas
        {
            get { return m_MetaDatas; }
        }

        public void Load()
        {
            if (MetaDatas == null)
                m_MetaDatas = new List<MetaData>();
            else 
				m_MetaDatas.Clear();
            
            var reader = new XMLReader.SSIReader();

            List<XMLReader.MetaData> metaData = new List<XMLReader.MetaData>();
            var all = new Dictionary<String, String>();

            foreach (var fi in getDlls())
            {
                try
                {
					System.Diagnostics.Debug.WriteLine(String.Format("Loading {0}", fi));
                    reader.ReadDll(fi).ForEach(c => all.Add(c, fi));
                }
                catch (Exception e)
                {

                }
            }

            foreach (var item in all.Keys)
            {
				XMLReader.MetaData md = null;
				
				try
				{
					System.Diagnostics.Debug.WriteLine(String.Format("Reading metadata of {0}", item));
					md = reader.ReadMetaData(item);
				}
				catch(Exception ex)
				{
				}
                
                if(md != null)
                {
					md.Lib = all[item];
					metaData.Add(md);
				}
            }

			var cacheObjs = new List<MetaDataCache>();
			var cache = new System.Runtime.Serialization.Formatters.Binary.BinaryFormatter();
			
			foreach(var md in m_MetaDatas)
				cacheObjs.Add(new MetaDataCache() { Data = md.Reference, Time = new FileInfo(md.Lib).CreationTime });
			
            metaData.ForEach(md => 
				{
					m_MetaDatas.Add(new MetaData(md));
					cacheObjs.Add(new MetaDataCache() { Data = md, Time = new FileInfo(all[md.Name]).CreationTime });
				});
				
#if DEBUG
			using(var fs = System.IO.File.Create("./xmleditorcached.bin"))
			{
				cache.Serialize(fs, cacheObjs);
            }
#else
            using (var fs = System.IO.File.Create("./xmleditorcache.bin"))
			{
				cache.Serialize(fs, cacheObjs);
            }
#endif
        }

        #endregion
        
        private List<string> getDlls()
        {
			List<MetaDataCache> mdCache = null;

            if (File.Exists("./xmledit.log"))
            {
                File.Delete("./xmledit.log");
            }
			
#if DEBUG
			if (System.IO.File.Exists("./xmleditorcached.bin"))
			{
				var serializer = new System.Runtime.Serialization.Formatters.Binary.BinaryFormatter();
				using(var fs = System.IO.File.OpenRead("./xmleditorcached.bin"))
				{
					mdCache = serializer.Deserialize(fs) as List<MetaDataCache>;
				}
			}
#else
            if (System.IO.File.Exists("./xmleditorcache.bin"))
			{
				var serializer = new System.Runtime.Serialization.Formatters.Binary.BinaryFormatter();
				using(var fs = System.IO.File.OpenRead("./xmleditorcache.bin"))
				{
					mdCache = serializer.Deserialize(fs) as List<MetaDataCache>;
				}
			}
#endif
			
			List<string> dlls = new List<string>();
            string[] files = System.IO.Directory.GetFiles(Properties.Settings.Default.Path, "ssi*.dll");     
			foreach (var fi in files)
            {
#if DEBUG
                if (!fi.EndsWith("d.dll"))
                    continue;
#else
                if (fi.EndsWith("d.dll"))
					continue;
#endif
					
				if(mdCache != null)
				{
					var result = mdCache.Where(c => c.Data.Lib.Equals(fi));
					if(result.Count() > 0)
					{
						var mdc = result.First();
						if(mdc.Time.Equals(new System.IO.FileInfo(fi).CreationTime))
						{
							foreach(var md in result)
								m_MetaDatas.Add(new MetaData(md.Data));
							
							continue;
						}
					}
				}
				
				dlls.Add(fi);			
			}
			
			return dlls;
		}
    }
}
